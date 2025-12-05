#include "Navigation.h"
#include "LineSensor.h"

#define DISTANCE_RALENTI 60
#define DISTANCE_OBSTACLE 22   // cm
#define VITESSE_AVANCE 120
#define VITESSE_RALENTI 120
#define VITESSE_VIRAGE 120
#define TEMPS_ROTATION 1000
#define CHECK_INTERVAL 10

// périodicités (ms)
#define SCAN_INTERVAL 200      // intervalle pour scan ultrasons
#define LINE_CHECK_INTERVAL 30 // fréquence contrôle ligne

// --- Motion scheduler (non-bloquant) ---
enum MotionType { MOT_NONE, MOT_FORWARD, MOT_BACKWARD, MOT_TURN_LEFT, MOT_TURN_RIGHT, MOT_LEFT, MOT_RIGHT };
struct Motion {
  MotionType type = MOT_NONE;
  int speed = 0;
  unsigned long endTime = 0;
  bool active = false;
} motion;

static unsigned long lastScanMs = 0;
static unsigned long lastLineCheckMs = 0;

// comportement : suivre la ligne située à gauche du robot
static unsigned long last_lost_ts = 0;
static const unsigned long LOST_TIMEOUT = 200; // ms avant recherche active

// start a motion for durationMs (non-blocking)
static void startMotion(MotionType t, int speed, unsigned long durationMs) {
  motion.type = t;
  motion.speed = speed;
  motion.endTime = millis() + durationMs;
  motion.active = true;

  switch (t) {
    case MOT_FORWARD:
      avancer(speed);
      break;
    case MOT_BACKWARD:
      reculer(speed); // sets direction pins
      break;
    case MOT_TURN_LEFT:
      tourner_gauche(speed);
      break;
    case MOT_TURN_RIGHT:
      tourner_droite(speed);
      break;
    case MOT_LEFT:
      avancer_gauche(speed);
      break;
    case MOT_RIGHT:
      avancer_droite(speed);
      break;
    default:
      stop();
      motion.active = false;
      break;
  }
}

// stop current motion now
static void stopMotionNow() {
  stop();
  motion.active = false;
  motion.type = MOT_NONE;
}

// must be called frequently to stop motion at endTime
static void updateMotion() {
  if (!motion.active) return;
  if (millis() >= motion.endTime) {
    stopMotionNow();
  }
}

static void periodicScan(long &distG, long &distC, long &distD) {
  // appel non-bloquant : Ultrason effectue une mesure en tâche de fond par update
  // il faut appeler ultrason_update() fréquemment depuis loop
  ultrason_update();
  // retourner les dernières valeurs connues
  distG = ultrason_getGauche();
  distC = ultrason_getCentre();
  distD = ultrason_getDroite();
  // print debug ponctuel (optionnel)
  // Serial.print("Scan G:"); Serial.print(distG); Serial.print(" C:"); Serial.print(distC); Serial.print(" D:"); Serial.println(distD);
}

// periodic line check -> corrective small motions (non-blocking)
static void periodicLineCheck() {
  unsigned long now = millis();
  if (now - lastLineCheckMs < LINE_CHECK_INTERVAL) return;
  lastLineCheckMs = now;

  // priorité : si détecte ligne -> corrections rapides et courtes
  if (droite_est_noir()) {
    // petite impulsion vers la droite
    startMotion(MOT_RIGHT, 160, 60);
    return;
  }
  if (milieu_est_noir()) {
    // avancer si pas déjà en avant
    if (!motion.active || motion.type != MOT_FORWARD) startMotion(MOT_FORWARD, VITESSE_AVANCE, 200);
    return;
  }
  if (gauche_est_noir()) {
    startMotion(MOT_LEFT, 160, 80);
    return;
  }

  // aucune ligne : avancer lentement pour rechercher
  if (!motion.active) startMotion(MOT_FORWARD, VITESSE_RALENTI, 200);
}

// non-bloquing avoidance sequence state-machine
static bool avoidActive = false;
static int avoidStep = 0;
static unsigned long avoidStepEnd = 0;
static long lastDistG=300, lastDistC=300, lastDistD=300;

static void startAvoidSequence() {
  avoidActive = true;
  avoidStep = 0;
  stopMotionNow();
  avoidStepEnd = millis(); // commence immédiatement
  Serial.println("Start avoid sequence");
}

static void processAvoidSequence() {
  if (!avoidActive) return;
  unsigned long now = millis();
  switch (avoidStep) {
    case 0:
      // step0 : recul court
      startMotion(MOT_BACKWARD, VITESSE_RALENTI, 300);
      avoidStepEnd = now + 300;
      avoidStep++;
      break;
    case 1:
      // wait for motion to finish
      if (now >= avoidStepEnd && !motion.active) { avoidStep++; }
      break;
    case 2:
      // tourner à droite court
      startMotion(MOT_TURN_RIGHT, 200, TEMPS_ROTATION);
      avoidStepEnd = now + TEMPS_ROTATION;
      avoidStep++;
      break;
    case 3:
      if (now >= avoidStepEnd && !motion.active) { avoidStep++; }
      break;
    case 4:
      // avancer par pas et tester droite libre
      startMotion(MOT_FORWARD, VITESSE_RALENTI, 500);
      avoidStepEnd = now + 500;
      avoidStep++;
      break;
    case 5:
      if (now >= avoidStepEnd && !motion.active) {
        // faire un scan (lever de servo et mesure)
        lastDistG = ultrason_getGauche();
        lastDistC = ultrason_getCentre();;
        lastDistD = ultrason_getDroite();
        Serial.print("Avoid scan G:"); Serial.print(lastDistG);
        Serial.print(" C:"); Serial.print(lastDistC);
        Serial.print(" D:"); Serial.println(lastDistD);

        if (lastDistG > DISTANCE_OBSTACLE) {
          // gauche dégagée -> tourner gauche pour revenir vers la ligne/obstacle
          startMotion(MOT_TURN_LEFT, 200, TEMPS_ROTATION);
          avoidStepEnd = now + TEMPS_ROTATION;
          avoidStep = 6;
        } else {
          // sinon, avancer encore
          avoidStep = 4; // boucle étape 4
        }
      }
      break;
    case 6:
      if (now >= avoidStepEnd && !motion.active) {
        // avancer un petit pas après rotation gauche
        startMotion(MOT_FORWARD, VITESSE_RALENTI, 400);
        avoidStepEnd = now + 400;
        avoidStep = 7;
      }
      break;
    case 7:
      if (now >= avoidStepEnd && !motion.active) {
        // fin de la séquence
        avoidActive = false;
        avoidStep = 0;
        Serial.println("Avoid sequence finished");
      }
      break;
    default:
      avoidActive = false;
      break;
  }
}

void navigation_init() {
  moteur_init();
  ultrason_init();
  line_init();
  lastScanMs = millis();
  lastLineCheckMs = millis();
  motion.active = false;
}


void navigation_loop() {
  // update scheduled motion
  updateMotion();

  // checks
  periodicLineCheck();

  // periodic scan (sets lastDist*)
  periodicScan(lastDistG, lastDistC, lastDistD);

  // obstacle detection based on latest scan results
  long minDist = min(lastDistC, min(lastDistG, lastDistD));
  if (minDist < DISTANCE_OBSTACLE && !avoidActive) {
    // start avoidance without blocking
    stopMotionNow();
    startAvoidSequence();
  }

  // progress avoidance state-machine (non-blocking)
  processAvoidSequence();

  // tiny yield
  delay(1);
}


/*
void eviter_obstacle_contourner() {
  Serial.println("Début contournement : tourner à droite");
  stop();
  delay(50);
  // tourner à droite pour commencer le contournement
  tourner_droite(200);
  delay(TEMPS_ROTATION);
  stop();
  delay(50);
  avancer(120);
  delay(200);

  while(!gauche_est_noir()) {
    // avancer en contournant : tant que l'obstacle est encore à gauche, avancer tout droit
    long dGauche = scanGauche(); // met à jour et attend stabilisation servo
    long dObstacle;
    if(dGauche > DISTANCE_OBSTACLE) {
      dObstacle = DISTANCE_OBSTACLE;
    }
    else {
      dObstacle = dGauche;
    }
    Serial.print("Contournement - dist gauche: "); Serial.println(dGauche);

    while(scanGauche() <= dObstacle + 5) {
      // obstacle toujours sur la gauche => on avance tout droit
      avancer(VITESSE_RALENTI);
    }
    if(scanGauche() <= dObstacle + 10) {
      stop();
      delay(2000);
      tourner_gauche(200);
      delay(50);
      avancer(VITESSE_RALENTI);
      delay(100);
    }
    else {
    // gauche dégagée => tourner à gauche pour revenir vers l'obstacle / la ligne
      Serial.println("gauche dégagée -> tourner à gauche pour retrouver obstacle ou ligne");
      delay(200);
      tourner_gauche(200);
      delay(TEMPS_ROTATION);
      stop();
      avancer(VITESSE_RALENTI);
      delay(1200);
      break;
    }
  }
  delay(50);
  Serial.println("Fin contournement");
}

*/

