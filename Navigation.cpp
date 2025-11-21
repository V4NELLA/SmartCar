#include "Navigation.h"

#define DISTANCE_OBSTACLE 40   // cm
#define VITESSE_AVANCE 100

void navigation_init() {
  moteur_init();
  ultrason_init();
}

void navigation_loop() {
  // --- Nouvelle logique : triple détection ---
  long distGauche = scanGauche();
  long distCentre = scanAvant();
  long distDroite = scanDroite();
  servomoteur.write(ANGLE_CENTRE);
  delay(200);

  Serial.print("Gauche: "); Serial.print(distGauche);
  Serial.print("  Centre: "); Serial.print(distCentre);
  Serial.print("  Droite: "); Serial.println(distDroite);

  // Choisir la plus petite distance (la plus proche)
  long distMin = min(distCentre, min(distGauche, distDroite));

  if (distMin < DISTANCE_OBSTACLE) {
    // Obstacle proche quelque part → éviter
    stop();
    delay(150);

    if (distDroite > distGauche) {
      tourner_droite(150);
      delay(450);
      stop();
      delay(100);
    } else {
      tourner_gauche(150);
      delay(450);
      stop();
      delay(100);
    }
    stop();
    delay(100);
  }
  else {
    // Pas d’obstacle → avancer
    avancer(VITESSE_AVANCE);
  }

  delay(50);
}
