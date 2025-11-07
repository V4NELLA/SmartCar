#include "Navigation.h"
#include "Logger.h"

void setup() {
  Serial.begin(9600);
  logger_init();
  navigation_init();
}

void loop() {
  navigation_loop();
}
