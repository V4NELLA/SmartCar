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
  int delai_pas; // délai par pas en ms pour le contrôle de vitesse

public:
  // Constructeur
  Pince(int pin, int ouvert = 90, int ferme = 0);
  
  // Initialisation
  void init();
  
  // Actions principales
  void ouvrir(int vitesse = -1);
  void fermer(int vitesse = -1);
  void arreter();
  void aller_position(int angle, int vitesse = -1);
  void lirePosition();
  
  // Réglage de la vitesse (délai entre pas en ms)
  void set_vitesse(int ms);
  void test_pince();
  
  // Utilitaire
  int get_position_actuelle();
};

#endif
