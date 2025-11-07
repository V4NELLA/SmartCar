#ifndef __ULTRASON_H__
#define __ULTRASON_H__

#include "SmartCar-v3Plus.h"

void ultrason_init();
long mesureDistance(); // distance devant
long scanDroite();
long scanGauche();
long scanAvant();

#endif