#include "Navigation.h"
#include "LineSensor.h"

#define DISTANCE_RALENTI 60
#define DISTANCE_OBSTACLE 22   // cm
#define VITESSE_AVANCE 120
#define VITESSE_RALENTI 120
#define VITESSE_VIRAGE 120
#define TEMPS_ROTATION 1000
#define CHECK_INTERVAL 10

// comportement : suivre la ligne située à droite du robot
static unsigned long last_lost_ts = 0;
static const unsigned long LOST_TIMEOUT = 200; // ms avant recherche active

#define DISTANCE_OBSTACLE 40   // cm
#define VITESSE_AVANCE 100

void navigation_init() {
  moteur_init();
  ultrason_init();
}

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

// maintien du suivi de ligne à droite (appel fréquent)
void suivre_ligne_gauche() {
  if(droite_est_noir()) {
    stop();
    reculer(50, 255);
    tourner_droite(200);
    delay(200);
    stop();
    last_lost_ts = 0;
  }
  else if(milieu_est_noir()) {
    avancer_droite(200);
    delay(30);
    last_lost_ts = 0;
  }
  else if(gauche_est_noir()) {
    avancer_gauche(200);
    delay(30);
    last_lost_ts = 0;
  }
  // aucun capteur ne voit la ligne : comportement de recherche
  if (last_lost_ts == 0) last_lost_ts = millis();
  unsigned long lost_for = millis() - last_lost_ts;

  if (lost_for < LOST_TIMEOUT) {
    // court: avancer doucement en espérant retrouver la ligne
    avancer(VITESSE_RALENTI);
    delay(30);
  } else {
    // perdu depuis trop longtemps : effectuer petite rotation droite pour chercher la ligne (on suit la droite)
    Serial.println("Ligne perdue : recherche active (rotation gauche)");
    tourner_gauche(200);
    delay(500);
    stop();
    while(!gauche_est_noir()) {
      avancer(VITESSE_RALENTI);
    }
    stop();
    delay(100);
  }
}

void detecter_obstacle() {
  // lire distance en début d'itération
  long distCentre = scanAvant();

  Serial.print(" C:"); Serial.print(distCentre);

  // Si obstacle devant ou proche -> lancer contournement
  if (distCentre < DISTANCE_OBSTACLE) {
    Serial.println("Obstacle detecte -> lancement contournement");
    avancer(120); //AJOUT car distance détection augmentée à 22cm
    delay(200);   //AJOUT car distance détection augmentée à 22cm
    stop();
    delay(50);
    eviter_obstacle_contourner();
    // après contournement, on reprend suivi ligne ; on laisse un petit délai
    delay(50);
    return;
  }

  delay(50);
}


void navigation_loop() {

  // Sinon : suivre la ligne située à droite (vérifications très fréquentes)
  suivre_ligne_gauche();
  detecter_obstacle();
 
}
