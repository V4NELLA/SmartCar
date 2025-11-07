// ...existing code...
#include "LineSensor.h"
#include "Logger.h"

// Seuils par défaut (valeurs typiques, à ajuster via calibration)
static int threshLeft = 400;
static int threshCenter = 400;
static int threshRight = 400;

void line_init() {
  pinMode(LSG, INPUT);
  pinMode(LSC, INPUT);
  pinMode(LSD, INPUT);

  // Auto-calibration rapide: lire quelques valeurs pour estimer fond clair
  long sL=0, sC=0, sR=0;
  const int N=10;
  delay(50);
  for (int i=0;i<N;i++){
    sL += analogRead(LSG);
    sC += analogRead(LSC);
    sR += analogRead(LSD);
    delay(20);
  }
  int avgL = sL/N;
  int avgC = sC/N;
  int avgR = sR/N;

  // On suppose que le fond est clair : seuil = avg - offset
  const int OFFSET = 150; // diminuer si capteur trop sensible
  threshLeft = max(50, avgL - OFFSET);
  threshCenter = max(50, avgC - OFFSET);
  threshRight = max(50, avgR - OFFSET);

  // log pour debug
  logPrint("Line init thresholds L:");
  logPrint(threshLeft);
  logPrint(" C:");
  logPrint(threshCenter);
  logPrint(" R:");
  logPrintln(threshRight);
}

void line_calibrate() {
  // Affiche valeurs brutes pour calibration manuelle (poser sur blanc puis noir)
  logPrintln("Line calibration: place sensor on WHITE then press reset to read WHITE values.");
  delay(2000);
  logPrint("WHITE L:");
  logPrint(getLeftRaw());
  logPrint(" C:");
  logPrint(getCenterRaw());
  logPrint(" R:");
  logPrintln(getRightRaw());

  logPrintln("Now place sensors on BLACK tape and restart to read BLACK values.");
  // L'utilisateur redémarrera et lira les valeurs.
}

bool isLineLeft() {
  return analogRead(LSG) < threshLeft;
}
bool isLineCenter() {
  return analogRead(LSC) < threshCenter;
}
bool isLineRight() {
  return analogRead(LSD) < threshRight;
}

int getLeftRaw()   { return analogRead(LSG); }
int getCenterRaw() { return analogRead(LSC); }
int getRightRaw()  { return analogRead(LSD); }
// ...existing code...