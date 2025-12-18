#ifndef __ULTRASON_H__
#define __ULTRASON_H__

#include "SmartCar-v3Plus.h"
#include <Servo.h>

void ultrason_init();            // init servo + IO
void ultrason_update();          // doit être appelé fréquemment (loop)
long ultrason_getGauche();       // dernière valeur lue (cm)
long ultrason_getCentre();
long ultrason_getDroite();

#endif