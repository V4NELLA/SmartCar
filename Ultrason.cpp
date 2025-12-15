#include "Ultrason.h"

//static Servo servomoteur;
static long lastD = 300;

static unsigned long us_deadline = 0;

static const unsigned long SERVO_SETTLE_MS = 220; // laisser stabiliser servo
static const unsigned long INTER_SCAN_MS = 20;    // délai entre mesures si besoin


void ultrason_init() {
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    servomoteur.attach(SERVO);
    servomoteur.write(ANGLE_DROITE);
    lastD = 300;
    us_deadline = millis();
}

// helper blocking pulse read (kept short timeout)
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

// state-machine: call frequently from loop (non-blocking except for short pulseIn)
void ultrason_update(bool centre) {
    unsigned long now = millis();
    if (centre) {
        // start moving to next angle
        servomoteur.write(ANGLE_CENTRE);
        us_deadline = now + SERVO_SETTLE_MS;
    }
    if (now >= us_deadline) {
      // measure (pulseIn blocks up to ~20ms)
      lastD = mesureDist();

      // small inter-scan gap
      us_deadline = now + INTER_SCAN_MS;
    }
}

void ultrason_lookDroite() {
    // remet le servomoteur en position droite et laisse le temps de stabiliser
    servomoteur.write(ANGLE_DROITE);
    us_deadline = millis() + SERVO_SETTLE_MS;
}

long ultrason_getDroite()  { return lastD; }

