#include "Ultrason.h"

static long lastG = 300, lastC = 300, lastD = 300;

enum US_Angle { US_CENTER = 0, US_RIGHT = 1, US_LEFT = 2 };
static const int angles[3] = { ANGLE_CENTRE, ANGLE_DROITE, ANGLE_GAUCHE };

// state machine
enum US_State { US_IDLE, US_MOVE, US_SETTLE, US_MEASURE };
static US_State us_state = US_IDLE;
static int us_index = 0;
static unsigned long us_deadline = 0;

static const unsigned long SERVO_SETTLE_MS = 220; // laisser stabiliser servo
static const unsigned long INTER_SCAN_MS = 50;    // délai entre mesures si besoin


void ultrason_init() {
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);
    servomoteur.attach(SERVO);
    servomoteur.write(ANGLE_CENTRE);
    lastG = lastC = lastD = 300;
    us_state = US_IDLE;
    us_index = 0;
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
void ultrason_update() {
    unsigned long now = millis();

    switch (us_state) {
      case US_IDLE:
        // début déplacement vers prohain angle
        servomoteur.write(angles[us_index]);
        us_deadline = now + SERVO_SETTLE_MS;
        us_state = US_SETTLE;
        break;

      case US_SETTLE:
        if (now >= us_deadline) {
          // mesure
          long d = mesureDist();
          if (us_index == US_CENTER) lastC = d;
          else if (us_index == US_RIGHT) lastD = d;
          else lastG = d;

          // preparer prochain angle
          us_index = (us_index + 1) % 3;
          us_state = US_IDLE;

          // court délai entre deux scan
          us_deadline = now + INTER_SCAN_MS;
        }
        break;

      default:
        us_state = US_IDLE;
        break;
    }
}

long ultrason_getGauche()  { return lastG; }
long ultrason_getCentre()  { return lastC; }
long ultrason_getDroite()  { return lastD; }
