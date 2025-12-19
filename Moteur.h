#ifndef __MOTEUR_H__
#define __MOTEUR_H__

#include "SmartCar-v3Plus.h"

void moteur_init();
void modifier_puissance(int puissance);
void avancer(int puissance);
void reculer();
void tourner_gauche(int p);
void tourner_droite(int p);
void avancer_gauche(int p);
void avancer_droite(int p);
void stop();

#endif