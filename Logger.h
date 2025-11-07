#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <Arduino.h>

void logger_init();
bool configureBT();
void logPrint(const String &s);
void logPrintln(const String &s);
void logPrint(const char *s);
void logPrintln(const char *s);
void logPrint(long v);
void logPrintln(long v);

#endif