#include "Pince.h"

// Constructeur : initialise les paramètres
Pince::Pince(int pin, int ouvert, int ferme) {
  pin_servo = pin;
  angle_ouvert = ouvert;
  angle_ferme = ferme;
  angle_actuel = ouvert;
  // délai par pas en ms (vitesse) par défaut
  delai_pas = 15;
}

// Initialise le servomoteur
void Pince::init() {
  servo_pince.attach(pin_servo);
  servo_pince.write(angle_ouvert);  // Position d'ouverture par défaut
  delay(100);
}

// Ouvre la pince (position = ouvert)
void Pince::ouvrir(int vitesse) {
  aller_position(angle_ouvert, vitesse);
}

// Ferme la pince (position = ferme)
void Pince::fermer(int vitesse) {
  aller_position(angle_ferme, vitesse);
}

// Arrête le servomoteur (neutre, généralement 90°)
void Pince::arreter() {
  servo_pince.write(90);  // Position neutre
  angle_actuel = 90;
}

// Déplace la pince vers un angle donné
void Pince::aller_position(int angle, int vitesse) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  
  int delai;
  if (vitesse <= 0) delai = delai_pas; else delai = vitesse;
  
  if (angle_actuel > angle) {
    // Descendre l'angle
    for (int pos = angle_actuel; pos >= angle; pos--) {
      servo_pince.write(pos);
      delay(delai);
    }
  } else if (angle_actuel < angle) {
    // Monter l'angle
    for (int pos = angle_actuel; pos <= angle; pos++) {
      servo_pince.write(pos);
      delay(delai);
    }
  }
  
  angle_actuel = angle;
}

// Retourne la position actuelle de la pince
int Pince::get_position_actuelle() {
  return angle_actuel;
}

void Pince::lirePosition() {
    Serial.print("Position pince : ");
    Serial.println(servo_pince.read());
}

// Réglage de la vitesse par défaut (délai entre pas en ms)
void Pince::set_vitesse(int ms) {
  if (ms < 0) ms = 0;
  delai_pas = ms;
}

void Pince::test_pince(){
  servo_pince.write(angle_ferme); 
  delay(2000);
  Serial.print("Angle fermé : ");
  Serial.println(servo_pince.read());
  servo_pince.write(angle_ouvert);
  delay(2000);
  Serial.print("Angle ouvert : ");
  Serial.println(servo_pince.read());
}