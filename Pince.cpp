#include "Pince.h"

// Constructeur : initialise les paramètres
Pince::Pince(int pin, int ouvert, int ferme) {
  pin_servo = pin;
  angle_ouvert = ouvert;
  angle_ferme = ferme;
  angle_actuel = ouvert;
}

// Initialise le servomoteur
void Pince::init() {
  servo_pince.attach(pin_servo);
  servo_pince.write(angle_ouvert);  // Position d'ouverture par défaut
  delay(100);
}

// Ouvre la pince (position = ouvert)
void Pince::ouvrir() {
  aller_position(angle_ouvert);
}

// Ferme la pince (position = ferme)
void Pince::fermer() {
  aller_position(angle_ferme);
}

// Arrête le servomoteur (neutre, généralement 90°)
void Pince::arreter() {
  servo_pince.write(90);  // Position neutre
  angle_actuel = 90;
}

// Déplace la pince vers un angle donné
void Pince::aller_position(int angle) {
  if (angle < 0) angle = 0;
  if (angle > 180) angle = 180;
  
  int delai = 15;
  
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