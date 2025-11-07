#include "Ultrason.h"

void ultrason_init() {
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    servomoteur.attach(SERVO);
    servomoteur.write(ANGLE_CENTRE);
}

long scanAvant() {
  // Le servo est déjà au centre normalement, mais on s'assure :
  servomoteur.write(ANGLE_CENTRE);
  delay(250); // attendre que le servo se stabilise
  return mesureDistance();
}

long mesureDistance() {
    long somme = 0;
    int n = 3; // nombre de mesures
    for (int i = 0; i < n; i++) {
        digitalWrite(TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);
        long duree = pulseIn(ECHO, HIGH, 20000); // Timeout 20 ms
        long dist = duree * 0.034 / 2;
        if (dist == 0 || dist > 300) dist = 300;
        somme += dist;
        delay(20);
    }
    return somme / n;
}


long scanDroite() {
    servomoteur.write(45);
    delay(100);
    return mesureDistance();
}

long scanGauche() {
    servomoteur.write(135);
    delay(300);
    return mesureDistance();
}
