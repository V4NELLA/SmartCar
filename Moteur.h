#ifndef __MOTEUR_H__
#define __MOTEUR_H__

#include "SmartCar-v3Plus.h"

void moteur_init();
void modifier_puissance(int puissance);
void avancer(int puissance);
void reculer();
void reculer(int duree);
void tourner_gauche();
void tourner_gauche(int duree);
void tourner_droite();
void tourner_droite(int duree);
void stop();

#endif