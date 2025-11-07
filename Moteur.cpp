#include "Moteur.h"

void moteur_init() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
}


void avancer(int p) {
	Serial.println("Avancer");
  analogWrite(ENA, p);
  analogWrite(ENB, p);
	digitalWrite(IN1,HIGH);
	digitalWrite(IN2,LOW);
	digitalWrite(IN3,LOW);
	digitalWrite(IN4,HIGH);
}

// Faire reculer le robot.
// pré-requis : gérer la puissance via modifier_puissance(int puissance)
void reculer(){
	Serial.println("Reculer");
	digitalWrite(IN1,LOW);
	digitalWrite(IN2,HIGH);
	digitalWrite(IN3,HIGH);
	digitalWrite(IN4,LOW);
}

// Idem, mais pour une durée donnée seulement
void reculer(int duree){
	reculer();
	delay(duree); // durée en ms
	stop();
}

// Faire trouner à gauche le robot.
// pré-requis : gérer la puissance via modifier_puissance(int puissance)
void tourner_gauche(int p){
	Serial.println("Tourner à gauche");
	analogWrite(ENA, p);
  	analogWrite(ENB, p);
	digitalWrite(IN1,LOW);
	digitalWrite(IN2,HIGH);
	digitalWrite(IN3,LOW);
	digitalWrite(IN4,HIGH); 
}


// Faire trouner à droite le robot.
// pré-requis : gérer la puissance via modifier_puissance(int puissance)
void tourner_droite(int p){
	analogWrite(ENA, p);
  	analogWrite(ENB, p);
	Serial.println("Tourner à droite");
	digitalWrite(IN1,HIGH);
	digitalWrite(IN2,LOW);
	digitalWrite(IN3,HIGH);
	digitalWrite(IN4,LOW); 
}


// Arrêt du robot
void stop() { // tout à LOW ou tout à 
	digitalWrite(IN1, LOW);      
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);      
	digitalWrite(IN4, LOW);
}