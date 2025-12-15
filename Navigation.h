#ifndef __NAVIGATION_H__
#define __NAVIGATION_H__

#include "Moteur.h"
#include "Ultrason.h"

void navigation_init();
void navigation_loop();
void periodicLineCheck();
void testPince(bool faireBouger);

#endif
