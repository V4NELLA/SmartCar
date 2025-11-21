#include "Navigation.h"
#include "LineSensor.h"

#define DISTANCE_RALENTI 60
#define DISTANCE_OBSTACLE 15   // cm
#define VITESSE_AVANCE 120
#define VITESSE_RALENTI 120
#define VITESSE_VIRAGE 120
#define TEMPS_ROTATION 1000
#define CHECK_INTERVAL 10

// comportement : suivre la ligne située à droite du robot
static unsigned long last_lost_ts = 0;
static const unsigned long LOST_TIMEOUT = 200; // ms avant recherche active


void navigation_init() {
  moteur_init();
  ultrason_init();
  line_init();
}

void eviter_obstacle_contourner() {
  Serial.println("Début contournement : tourner à gauche");
  stop();
  delay(50);
  // tourner à gauche pour commencer le contournement
  tourner_gauche(200);
  delay(TEMPS_ROTATION);
  stop();
  delay(50);
  avancer(120);
  delay(200);

  while(!droite_est_noir()) {
    // avancer en contournant : tant que l'obstacle est encore à droite, avancer tout droit
    long dDroite = scanDroite(); // met à jour et attend stabilisation servo
    long dObstacle;
    if(dDroite > DISTANCE_OBSTACLE) {
      dObstacle = DISTANCE_OBSTACLE;
    }
    else {
      dObstacle = dDroite;
    }
    Serial.print("Contournement - dist droite: "); Serial.println(dDroite);

    while(scanDroite() <= dObstacle + 5) {
      // obstacle toujours sur la droite => on avance tout droit
      avancer(VITESSE_RALENTI);
    }
    if(scanDroite() <= dObstacle + 10) {
      stop();
      delay(2000);
      tourner_droite(200);
      delay(50);
      avancer(VITESSE_RALENTI);
      delay(100);
    }
    else {
    // droite dégagée => tourner à droite pour revenir vers l'obstacle / la ligne
      Serial.println("droite dégagée -> tourner à droite pour retrouver obstacle ou ligne");
      delay(400);
      tourner_droite(200);
      delay(TEMPS_ROTATION*0.8);
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
void suivre_ligne_droite() {
  if(gauche_est_noir()) {
    stop();
    reculer(50, 255);
    tourner_gauche(200);
    delay(200);
    stop();
    last_lost_ts = 0;
  }
  else if(milieu_est_noir()) {
    avancer_gauche(200);
    delay(30);
    last_lost_ts = 0;
  }
  else if(droite_est_noir()) {
    avancer_droite(200);
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
    Serial.println("Ligne perdue : recherche active (rotation droite)");
    tourner_droite(200);
    delay(500);
    stop();
    while(!droite_est_noir()) {
      avancer(VITESSE_RALENTI);
    }
    stop();
    last_lost_ts = 0;
  }
}

void detecter_obstacle() {
  // lire distance en début d'itération
  long distCentre = scanAvant();

  Serial.print(" C:"); Serial.print(distCentre);

  // Si obstacle devant ou proche -> lancer contournement
  if (distCentre < DISTANCE_OBSTACLE) {
    Serial.println("Obstacle detecte -> lancement contournement");
    stop();
    delay(50);
    eviter_obstacle_contourner();
    // après contournement, on reprend suivi ligne ; on laisse un petit délai
    delay(50);
    return;
  }

  // petit délai pour stabilité (CHECK_INTERVAL permet fréquence élevée)
  delay(CHECK_INTERVAL);
}


void navigation_loop() {

  // Sinon : suivre la ligne située à droite (vérifications très fréquentes)
  suivre_ligne_droite();
  detecter_obstacle();
 
}