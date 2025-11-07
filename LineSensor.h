// ...existing code...
#ifndef __LINESENSOR_H__
#define __LINESENSOR_H__

#include <Arduino.h>


// Pins (modifiez si nécessaire)
#define LSG  2
#define LSC 4
#define LSD 10

void line_init();               // initialise et calibre rapidement
void line_calibrate();          // affiche valeurs pour calibration manuelle
bool isLineLeft();
bool isLineCenter();
bool isLineRight();
int getLeftRaw();
int getCenterRaw();
int getRightRaw();

#endif
// ...existing code...