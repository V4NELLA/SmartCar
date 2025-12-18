#include "Ultrason.h"

static long lastD = 300;

static unsigned long us_deadline = 0;

static const unsigned long SERVO_SETTLE_MS = 220; // delay pour stabiliser position servo
static const unsigned long INTER_SCAN_MS = 20;    // délai entre mesures en millisecondes


void ultrason_init() {
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    servomoteur.attach(SERVO);
    servomoteur.write(ANGLE_DROITE);
    lastD = 300;
    us_deadline = millis();
}

// Fonction de scan retournant la distance en centimètres
static long mesureDist() {
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    long duree = pulseIn(ECHO, HIGH, 20000); // 20 ms timeout
    long dist = duree * 0.034 / 2;
    if (dist == 0 || dist > 300) dist = 300;
    return dist;
}

// Fonction non bloquante gérant les déplacements et l'appel de la fonction scan de l'ultrason
void ultrason_update(bool centre) {
    unsigned long now = millis();
    if (centre) {
        // Lorsque la robot est en approche de l'objet à saisir, le capteur ultrason doit regarder devant (condition "centre")
        servomoteur.write(ANGLE_CENTRE);
        us_deadline = now + SERVO_SETTLE_MS;
    }
    if (now >= us_deadline) {
      // Prise de la mesure
      lastD = mesureDist();

      // Attente du délai minimum avant prochain scan
      us_deadline = now + INTER_SCAN_MS;
    }
}

void ultrason_Droite() {
    // remet le servomoteur en position droite et laisse le temps de stabiliser
    servomoteur.write(ANGLE_DROITE);
    us_deadline = millis() + SERVO_SETTLE_MS;
}

// Fonction pour retourner la distance scannée
long ultrason_getDroite()  { return lastD; }

