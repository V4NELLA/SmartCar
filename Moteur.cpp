#include "Moteur.h"

// Ajout : trim pour corriger décalage latéral
static int motor_trim = -40; // positif => augmente puissance moteur "gauche" (ENA), négatif => augmente droite (ENB)


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
void reculer(int p){
	Serial.println("Reculer");
	analogWrite(ENA, p);
	analogWrite(ENB, p);
	digitalWrite(IN1,LOW);
	digitalWrite(IN2,HIGH);
	digitalWrite(IN3,HIGH);
	digitalWrite(IN4,LOW);
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


void avancer_droite(int p) {
    // tourne légèrement vers la droite en réduisant la roue droite
    int left = constrain(p + motor_trim, 0, 255);
    int right = constrain(p - motor_trim, 0, 255);
    right = (right * 50) / 100; // réduire roue intérieure (droite)

    analogWrite(ENA, left);   // PWM gauche
    analogWrite(ENB, right);  // PWM droite

    // direction avant (mêmes pin que dans avancer())
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    Serial.println("Avancer droite");
}

void avancer_gauche(int p) {
    // tourne légèrement vers la gauche en réduisant la roue gauche
    int left = constrain(p + motor_trim, 0, 255);
    int right = constrain(p - motor_trim, 0, 255);
    left = (left * 32) / 100; // réduire roue intérieure (gauche)

    analogWrite(ENA, left);   // PWM gauche
    analogWrite(ENB, right);  // PWM droite

    // direction avant
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    Serial.println("Avancer gauche");
}


// Arrêt du robot
void stop() { // tout à LOW ou tout à 
	digitalWrite(IN1, LOW);      
	digitalWrite(IN2, LOW);
	digitalWrite(IN3, LOW);      
	digitalWrite(IN4, LOW);
	analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}