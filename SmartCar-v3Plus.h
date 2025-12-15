#ifndef __SMARTCAR_V3PLUS_HH__
#define __SMARTCAR_V3PLUS_HH__

// HEADERS et configuration pour Robot "Car V3 Plus" chez Elegoo (www.elegoo.com)
// Thomas Raimbault, 09/10/2025

#include <Arduino.h> // nécessaire parce que utilise des trucs Arduino
#include <Servo.h> 

// Il est préférable de faire une pause, avant un changement d'actions (avant de tourner, avant de tester une distance, avant de redémarrer, ...)
#define DELAY_BEFORE_NEW_ACTION 100

//-----------------------------------------------------------------------------
// Header pour Déplacements
//-----------------------------------------------------------------------------

//define L298n module IO Pin (motor controller)
#define ENA 5
#define ENB 6
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 11

// Rq : la puissance minimum dépend de la nature du sol (généralement au environ de 100)
#define PUISSANCE_MAX 255


//-----------------------------------------------------------------------------
// Header pour Capteur de distance à ultrasons
//-----------------------------------------------------------------------------

// La distance étant déterminée par le temps de propagation des ultrasons
// entre l'émetteur ECHO et le récepteur TRIG.
// Define Pins
#define ECHO A4
#define TRIG A5

// Define Pin for servo
#define SERVO 3

// Variable globale
static Servo servomoteur; // objet pour contrôler le servomoteur où est fixé le capteur de distance

// configuration additionnelle :
// angles min et max réels applicables ; la valeur 0° force le servomoteur
#define ANGLE_DROITE 50
#define ANGLE_CENTRE 90
#define ANGLE_GAUCHE 170


//-----------------------------------------------------------------------------
// Header pour Suiveur de ligne
//-----------------------------------------------------------------------------

//define Line Tracking IO (Pins for Right, Middle, and Left trackers)
#define LTR 10
#define LTM 4
#define LTL 2


//-----------------------------------------------------------------------------
// En-têtes (Header) des fonctions fournies
//-----------------------------------------------------------------------------

void initialisation_RobotCar();

void modifier_puissance(int puissance);
void stop();
void avancer();
void avancer(int duree);
void reculer();
void reculer(int duree);
void tourner_gauche();
void tourner_gauche(int duree);
void tourner_droite();
void tourner_droite(int duree);

void modifier_angle_vision(int angle);
int determiner_distance_obstacle();

bool milieu_est_noir();
bool droite_est_noir();
bool gauche_est_noir();

bool obstacle();
void modifier_chemin();

#endif
