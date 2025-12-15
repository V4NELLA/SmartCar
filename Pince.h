#ifndef PINCE_H
#define PINCE_H

#include <Arduino.h>
#include <Servo.h>

class Pince {
private:
  Servo servo_pince;
  int pin_servo;
  int angle_ouvert;
  int angle_ferme;
  int angle_actuel;

public:
  // Constructeur
  Pince(int pin, int ouvert = 90, int ferme = 0);
  
  // Initialisation
  void init();
  
  // Actions principales
  void ouvrir();
  void fermer();
  void arreter();
  void aller_position(int angle);
  
  // Utilitaire
  int get_position_actuelle();
};

#endif
