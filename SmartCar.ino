/*
Logique d'algo : Suivre la ligne sur la droite du robot à l'aide d'un triple capteur IR de suivi de ligne + détection d'obstacle
avec fonction de contournement (tourner sur la gauche puis tant que obstacle sur la droite aller tout droit sinon tourner à droite pour retrouver
l'obstacle ou la ligne).
*/

#include "Navigation.h"

void setup() {
  Serial.begin(9600);
  navigation_init();
}

void loop() {
  navigation_loop();
}