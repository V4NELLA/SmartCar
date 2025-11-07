#include "Navigation.h"
#include "Logger.h"
#include "LineSensor.h"

#define DISTANCE_RALENTI 80
#define DISTANCE_OBSTACLE 10   // cm
#define VITESSE_AVANCE 140
#define VITESSE_RALENTI 80
#define TEMPS_ROTATION 300 

void navigation_init() {
  moteur_init();
  ultrason_init();
  line_init();
}

void navigation_loop() {

  // --- Nouvelle logique : triple détection ---
  long distGauche = scanGauche();
  long distCentre = scanAvant();
  long distDroite = scanDroite();

  logPrint("Gauche: ");
  logPrint(distGauche);
  logPrint("  Centre: ");
  logPrint(distCentre);
  logPrint("  Droite: ");
  logPrintln(distDroite);
  //Serial.print("Gauche: "); Serial.print(distGauche);
  //Serial.print("  Centre: "); Serial.print(distCentre);
  //Serial.print("  Droite: "); Serial.println(distDroite);

 // Si obstacle détecté devant
  if ((distCentre < DISTANCE_OBSTACLE) || (distGauche < DISTANCE_OBSTACLE) || (distDroite < DISTANCE_OBSTACLE)) {
      stop();
      delay(200);  // Pause plus longue pour assurer l'arrêt

    // Choisir la direction avec le plus d'espace
    if (distDroite > distGauche && distDroite > DISTANCE_OBSTACLE) {
        tourner_droite(250);
        delay(TEMPS_ROTATION);
    } else if (distGauche > DISTANCE_OBSTACLE) {
        tourner_gauche(250);
        delay(TEMPS_ROTATION);
    } else {
        // Si bloqué des deux côtés, reculer et tourner
        reculer();
        delay(100);
        tourner_droite(250);
        delay(TEMPS_ROTATION);
    }
    stop();
    delay(200);
  }
  // Ralentir si on approche d'un obstacle
  else if (distCentre <= DISTANCE_RALENTI) {
      avancer(VITESSE_AVANCE);
      delay(50);
      avancer(VITESSE_RALENTI);
  }
  // Sinon avancer normalement
  else {
      avancer(VITESSE_AVANCE);
  }

  delay(50);
}
