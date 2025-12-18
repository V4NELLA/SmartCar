#include "Navigation.h"

void setup() {
  Serial.begin(9600);
  navigation_init();
}

void loop() {
  navigation_loop();
  //testPince();
}
