// ...existing code...
#ifndef __LINESENSOR_H__
#define __LINESENSOR_H__

#include <Arduino.h>


// Pins (modifiez si nécessaire)
#define LSG  2
#define LSC 4
#define LSD 10

void line_init();               // initialise et calibre rapidement
bool milieu_est_noir();
bool droite_est_noir();
bool gauche_est_noir();

#endif
// ...existing code...