#ifndef __MOTEUR_H__
#define __MOTEUR_H__

#include "SmartCar-v3Plus.h"

void modifier_puissance(int trim);
void moteur_init();
void modifier_puissance(int puissance);
void avancer(int puissance);
void reculer();
void reculer(int duree, int p);
void tourner_gauche();
void tourner_gauche(int duree);
void tourner_droite();
void tourner_droite(int duree);
void avancer_droite(int p);
void avancer_gauche(int p);
void stop();

#endif