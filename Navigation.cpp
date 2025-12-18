#include "Navigation.h"
#include "LineSensor.h"
#include "Pince.h"

// Paramètres mouvements du robot
#define VITESSE_AVANCE 200
#define TEMPS_ROTATION 350

// périodicités (ms)
#define SCAN_INTERVAL 20      // intervalle pour scan ultrasons
#define LINE_CHECK_INTERVAL 30 // intervalle contrôle ligne

// pour prise d'objet
#define OBJECT_DETECT_DISTANCE 35  // distance pour déclencher séquence de prise (cm)
#define DISTANCE_SAISIE 20            // distance cible pour fermer la pince (cm)
#define APPROCHE_TIMEOUT 1000      // timeout sécurité pour approche (ms)

Pince pince(13, 0, 180);  // Pin 13, 0° ouvert, 180° fermé

// --- Gestion des mouvements  (non-bloquant) ---
enum MotionType { MOT_NONE, MOT_FORWARD, MOT_BACKWARD, MOT_TURN_LEFT, MOT_TURN_RIGHT, MOT_LEFT, MOT_RIGHT };
struct Motion {
  MotionType type = MOT_NONE;
  int speed = 0;
  unsigned long endTime = 0;
  bool active = false;
} motion;

static unsigned long lastScanMs = 0;
static unsigned long lastLineCheckMs = 0;

static long lastDistD=300, lastDistC = 300;

static bool pickActive = false;   // Booléen d'état pour mener la saisie de l'objet
static bool centre = false;       // Booléen permettant de gérer la direction de scan du capteur ultrason
static bool deja_recup = false;   // Vrai si objet délà saisi
static int pickStep = 0;          // Variable indiquant les différentes étapes de saisie dans le switch de saisie
static unsigned long pickStepEnd = 0;   // Variable stockant le temps millis de fin d'étape (saisie)
static unsigned long pickApproachStart = 0;   // Variable stockant le temps millis de débit d'approche pour saisie objet

// Fonction pour débuter un mouvement pendant une période définie (non-bloquant)
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
      reculer(speed);
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

// Arrêt du mouvement courant
static void stopMotionNow() {
  stop();
  motion.active = false;
  motion.type = MOT_NONE;
}

// Vérifie à intervalle régulier l'état de mouvement
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


// Fonction de correction du mouvement en fonction du check de ligne (non-bloquante)
static void periodicLineCheck() {

  Serial.println("Scan ligne");
  unsigned long now = millis();
  if (now - lastLineCheckMs < LINE_CHECK_INTERVAL) return;
  lastLineCheckMs = now;

  // priorité : si détecte ligne -> corrections rapides et courtes
  if (droite_est_noir()) {
    // impulsion vers la droite
    Serial.println("Droite est noir");
    startMotion(MOT_TURN_RIGHT, 200, 100);
    return;
  }
  if (milieu_est_noir()) {
    // avancer si pas déjà en avant
    if (!motion.active || motion.type != MOT_FORWARD) startMotion(MOT_FORWARD, VITESSE_AVANCE, 200);
    return;
  }
  if (gauche_est_noir()) {
    // tourner vers la gauche
    startMotion(MOT_TURN_LEFT, 160, 80);
    return;
  }

  // aucune ligne : avancer vers la gauche tout en avançant pour retrouver la ligne
  if (!motion.active && !pickActive) startMotion(MOT_LEFT, VITESSE_AVANCE, 200);
}


/*
// Fonction de suivi de ligne avec un seul capteur
static void periodicLineCheck() {

  Serial.println("Scan ligne");
  unsigned long now = millis();
  if (now - lastLineCheckMs < LINE_CHECK_INTERVAL) return;
  lastLineCheckMs = now;

  // priorité : si détecte ligne -> corrections rapides et courtes
  if (gauche_est_noir()) {
    // petite impulsion vers la droite
    Serial.println("Droite est noir");
    startMotion(MOT_TURN_RIGHT, 160, 60);
    return;
  }

  // aucune ligne : avancer lentement pour rechercher
  if (!motion.active && !avoidActive) startMotion(MOT_LEFT, VITESSE_AVANCE, 200);
}
*/


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
  switch (pickStep) {
    case 0:
      // ouvrir la pince
      pince.ouvrir(8); // vitesse (ms par pas) raisonnable
      pickStep++;
      pickStepEnd = now + 200;
      break;
    case 1:
      if (now >= pickStepEnd && !motion.active) { pickStep++; }
      break;
    case 2:
      // tourner sur la droite en direction de l'objet à récupérer
      startMotion(MOT_TURN_RIGHT, VITESSE_AVANCE, TEMPS_ROTATION);
      pickStepEnd = now + TEMPS_ROTATION;
      pickStep++;
      break;
    case 3:
      if (now >= pickStepEnd && !motion.active) { pickStep++; }
      break;
    case 4:
      // approche vers l'objet
      pickApproachStart = now;
      startMotion(MOT_FORWARD, VITESSE_AVANCE, APPROCHE_TIMEOUT);
      pickStep++;
      break;
    case 5:
      // si trop proche ou timeout, arrêter et fermer pince
      if (lastDistC <= DISTANCE_SAISIE || (!motion.active) || (now - pickApproachStart > APPROCHE_TIMEOUT)) {
        stopMotionNow();
        delay(50);
        pince.fermer(8); // fermer
        pickStep++;
        pickStepEnd = now + 300;
      }
      break;
    case 6:
      // reculer après saisie
      if (now >= pickStepEnd) {
        startMotion(MOT_BACKWARD, VITESSE_AVANCE, APPROCHE_TIMEOUT);
        pickStep++;
        pickStepEnd = now + APPROCHE_TIMEOUT;
      }
      break;
    case 7:
      if (now >= pickStepEnd && !motion.active) { pickStep++; }
      break;
    case 8:
      // tourner sur la gauche pour retrouver la ligne
      startMotion(MOT_TURN_LEFT, VITESSE_AVANCE, TEMPS_ROTATION);
      pickStepEnd = now + TEMPS_ROTATION;
      pickStep++;
      break;
    case 9:
      if (now >= pickStepEnd && !motion.active) {
        // fin
        pickActive = false;
        pickStep = 0;
        // remettre ultrasonic à droite après prise
        ultrason_Droite();
        centre = false;
        deja_recup = true;
        Serial.println("Pick sequence finished");
      }
      break;
    default:
      pickActive = false;
      break;
  }
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
  // actualiser la fonction de déplacement
  updateMotion();

  // Appel de la fonction de détection de ligne si pas en phase de saisie
  if(!pickActive) {
    periodicLineCheck();
  }

  // appel fonction scan ultrason
  periodicScan(lastDistD, centre);

  // object detection: centre plus proche que côtés => lancer séquence de prise
  if (!pickActive && !deja_recup) {
    if (lastDistD < OBJECT_DETECT_DISTANCE) {
      // Serial.print("Objet detecte C=");
      // Serial.println(lastDistD);
      startPickSequence();
    }
  }

  // appel de la fonctio à état de l'étape de saisie (non-bloquante)
  processPickSequence();

  delay(1);
}