#include "Navigation.h"
#include "LineSensor.h"
#include "Pince.h"

#define DISTANCE_RALENTI 60
#define DISTANCE_OBSTACLE 25   // cm
#define VITESSE_AVANCE 200
#define VITESSE_RALENTI 180
#define VITESSE_VIRAGE 120
#define TEMPS_ROTATION 350
#define CHECK_INTERVAL 30

// périodicités (ms)
#define SCAN_INTERVAL 20      // intervalle pour scan ultrasons
#define LINE_CHECK_INTERVAL 30 // fréquence contrôle ligne

// pour prise d'objet
#define OBJECT_DETECT_DISTANCE 35  // distance pour déclencher séquence de prise (cm)
#define GRAB_DISTANCE 20            // distance cible pour fermer la pince (cm)
#define APPROACH_TIMEOUT 1000      // timeout sécurité pour approche (ms)

Pince pince(13, 0, 180);  // Pin 13, 170° ouvert, 10° fermé

// --- Motion scheduler (non-bloquant) ---
enum MotionType { MOT_NONE, MOT_FORWARD, MOT_BACKWARD, MOT_BACK_LEFT, MOT_TURN_LEFT, MOT_TURN_RIGHT, MOT_LEFT, MOT_RIGHT };
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

static long lastDistD=300, lastDistC = 300;

// nouvelle state-machine de prise d'objet
static bool pickActive = true;
static bool centre = false;
static bool lestart = true;
static int pickStep = 0;
static unsigned long pickStepEnd = 0;
static unsigned long pickApproachStart = 0;

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
    case MOT_BACK_LEFT:
      reculer_gauche(speed);
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

static void periodicScan(long &distD, bool centre) {
  // appel non-bloquant : Ultrason effectue une mesure en tâche de fond par update
  ultrason_update(centre);
  // retourner les dernières valeurs connues
  distD = ultrason_getDroite();
  Serial.print(" D:"); Serial.println(distD);
}


// periodic line check -> corrective small motions (non-blocking)
static void periodicLineCheck() {

  Serial.println("Scan ligne");
  unsigned long now = millis();
  if (now - lastLineCheckMs < LINE_CHECK_INTERVAL) return;
  lastLineCheckMs = now;

  if (gauche_est_noir()) {
    if(pickActive){
      if(lestart) {
        pickActive = false;
      }
      else {
        lestart = true;
      }
    }
    else {
      pickActive = true;
    }
    return;
  }

  // aucune ligne : avancer pour récupérer
  if (pickActive) startMotion(MOT_LEFT, VITESSE_AVANCE, 200);
  if (!pickActive) startMotion(MOT_BACK_LEFT, VITESSE_AVANCE, 200);
}


// --- nouvelle séquence non-bloquante de prise d'objet ---
static void startPickSequence() {
  if (pickActive) return;
  pickActive = true;
  centre = true;
  pickStep = 0;
  pickStepEnd = millis();
  stopMotionNow();
  Serial.println("Start pick sequence");
}

static void processPickSequence() {
  if (!pickActive) return;
  unsigned long now = millis();
    // tourner sur la droite en direction de l'objet à récupérer
    startMotion(MOT_LEFT, VITESSE_AVANCE, TEMPS_ROTATION);
    pickStepEnd = now + TEMPS_ROTATION;
    pickStep++;
    if (now >= pickStepEnd && !motion.active) { pickStep++; }
}


void navigation_init() {
  moteur_init();
  ultrason_init();
  line_init();
  pince.init();
  lastScanMs = millis();
  lastLineCheckMs = millis();
  motion.active = false;
}


void navigation_loop() {
  // update scheduled motion
  updateMotion();

  // checks
  periodicLineCheck();

  // progress pick state-machine (non-blocking)
  processPickSequence();

  delay(1);
}
