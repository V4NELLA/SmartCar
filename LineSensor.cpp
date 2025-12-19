#include "LineSensor.h"


void line_init() {
  pinMode(LSG, INPUT);
  pinMode(LSC, INPUT);
  pinMode(LSD, INPUT);
}


// Capteur central
// retourne true si noir (ou pas de détection), false sinon
bool milieu_est_noir() {
	return digitalRead(LSC) == LOW;
}

// Capteur droite
// retourne true si noir (ou pas de détection), false sinon
bool droite_est_noir() {
	return digitalRead(LSD) == LOW;
}



// Capteur gauche
// retourne true si noir (ou pas de détection), false sinon
bool gauche_est_noir() {
	return digitalRead(LSG) == LOW;
}