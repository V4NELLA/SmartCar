#include "Ultrason.h"

//static Servo servomoteur;
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
void ultrason_update() {
    unsigned long now = millis();

    switch (us_state) {
      case US_IDLE:
        // start moving to next angle
        servomoteur.write(angles[us_index]);
        us_deadline = now + SERVO_SETTLE_MS;
        us_state = US_SETTLE;
        break;

      case US_SETTLE:
        if (now >= us_deadline) {
          // measure (pulseIn blocks up to ~20ms)
          long d = mesureDist();
          if (us_index == US_CENTER) lastC = d;
          else if (us_index == US_RIGHT) lastD = d;
          else /* LEFT */ lastG = d;

          // prepare next angle
          us_index = (us_index + 1) % 3;
          us_state = US_IDLE;

          // small inter-scan gap
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

void ultrason_requestFullScan() {
  // force immediate cycle: set index to centre so update will measure soon
  us_index = 0;
  us_state = US_IDLE;
  us_deadline = millis();
}

/*
long scanAvant() {
  // Le servo est déjà au centre normalement, mais on s'assure :
  servomoteur.write(ANGLE_CENTRE);
  //Serial.print("Position  scan avant : "); Serial.println(servomoteur.read());
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
    //Serial.print("Position  mesure distance : "); Serial.println(servomoteur.read());
    return somme / n;
}


long scanDroite() {
    servomoteur.write(ANGLE_DROITE);
    delay(300);
    return mesureDistance();
}

long scanGauche() {
    servomoteur.write(180);
    delay(100);
    Serial.print("Position  scan gauche : "); Serial.println(servomoteur.read());
    return mesureDistance();
}
*/