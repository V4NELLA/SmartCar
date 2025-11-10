#include "Navigation.h"
#include "LineSensor.h"

#define DISTANCE_RALENTI 60
#define DISTANCE_OBSTACLE 15   // cm
#define VITESSE_AVANCE 130
#define VITESSE_RALENTI 100
#define TEMPS_ROTATION 200
#define CHECK_INTERVAL 10

void navigation_init() {
  moteur_init();
  ultrason_init();
  line_init();
}

void corrective_from_line() {
  // Action corrective simple : si capteur sur la bande noire, reculer + tourner vers intérieur
  if (gauche_est_noir()) {
    Serial.println("Ligne detectee: gauche -> correction droite");
    stop();
    delay(50);
    while(gauche_est_noir()) {
      tourner_droite(200);
    }
    stop();
  } else if (droite_est_noir()) {
    Serial.println("Ligne detectee: droite -> correction gauche");
    stop();
    delay(50);
    while(droite_est_noir()) {
      tourner_gauche(200);
    }
    stop();
  } else if (milieu_est_noir()) {
    // si au centre, on est trop près de la limite : reculer et tourner aléatoirement vers l'intérieur
    Serial.println("Ligne detectee: centre -> reculer");
    stop();
    delay(50);
    reculer(200);
    tourner_gauche(250);
    delay(TEMPS_ROTATION);
    stop();
  }
}

void check_line() {
  if (gauche_est_noir() || droite_est_noir() || milieu_est_noir()) {
    corrective_from_line();
  }
}

void avancer_avec_check(int vitesse, int duree) {
  unsigned long startTime = millis();
  while (millis() - startTime < duree) {
    avancer(vitesse);
    check_line();  // Vérifie la ligne pendant l'avance
    delay(CHECK_INTERVAL);
  }
}

void tourner_avec_check(bool vers_droite, int vitesse, int duree) {
  unsigned long startTime = millis();
  while (millis() - startTime < duree) {
    if (vers_droite) {
      tourner_droite(vitesse);
    } else {
      tourner_gauche(vitesse);
    }
    check_line();
    delay(CHECK_INTERVAL);
  }
}



void navigation_loop() {
  long distGauche, distCentre, distDroite;

  stop();

  distGauche = scanGauche();
  distCentre = scanAvant();
  distDroite = scanDroite();

  Serial.print("G:"); Serial.print(distGauche);
  Serial.print(" C:"); Serial.print(distCentre);
  Serial.print(" D:"); Serial.println(distDroite);

  // Vérifier la ligne en priorité
  check_line();

  // Gestion des obstacles
  if ((distDroite < DISTANCE_OBSTACLE) || (distGauche < DISTANCE_OBSTACLE) || (distCentre < DISTANCE_OBSTACLE)) {
      stop();
      delay(50);

      // Choix de la direction d'évitement
      if (distDroite > distGauche && distDroite > DISTANCE_OBSTACLE) {
          tourner_avec_check(true, 250, TEMPS_ROTATION);
      } else if (distGauche > DISTANCE_OBSTACLE) {
          tourner_avec_check(false, 250, TEMPS_ROTATION);
      } else {
          // Bloqué : recul et rotation
          reculer();
          delay(200);
          tourner_avec_check(true, 250, TEMPS_ROTATION);
      }
      stop();
  }
  // Navigation normale avec vérification ligne continue
  else if (distCentre <= DISTANCE_RALENTI) {
      avancer_avec_check(VITESSE_RALENTI, 1000);
  } else {
      avancer_avec_check(VITESSE_AVANCE, 1000);
  }
  delay(10);  // Court délai pour stabilité
}