#include "Logger.h"
#include <SoftwareSerial.h>

// RX, TX pour SoftwareSerial : RX = Arduino pin connecté au BT TX, TX = Arduino pin connecté au BT RX
static SoftwareSerial btSerial(0, 1);

void logger_init() {
    pinMode(0, INPUT);
    pinMode(1, OUTPUT);
    btSerial.begin(9600);
    delay(50);
    btSerial.println("BT ready");
}

void logPrint(const String &s) {
    Serial.print(s);
    btSerial.print(s);
}
void logPrintln(const String &s) {
    Serial.println(s);
    btSerial.println(s);
}
void logPrint(const char *s) {
    Serial.print(s);
    btSerial.print(s);
}
void logPrintln(const char *s) {
    Serial.println(s);
    btSerial.println(s);
}
void logPrint(long v) {
    Serial.print(v);
    btSerial.print(v);
}
void logPrintln(long v) {
    Serial.println(v);
    btSerial.println(v);
}