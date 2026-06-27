// ===== Ultrasonic proximity sensor (HC-SR04) =====
// Used to notice when a hand is reaching in, so the king can duck back before you touch him.

#include "Config.h"

// Ping once and return the distance in cm. Returns a big number on timeout (nothing there).
long readDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 30000);  // 30 ms timeout (~5 m max)
  if (duration == 0) return 999;                   // nothing bounced back
  return duration / 58;                            // echo time -> cm
}

// True if something is within MOVEMENT_CM (5 cm) - close enough to count as "reaching in".
bool sensorSeesMovement() {
  return readDistanceCm() < MOVEMENT_CM;
}

// Turn a distance into a buzzer frequency so the tone "follows" your hand:
// hand close (MOVEMENT_CM) -> hiFreq, hand far (SENSE_FAR_CM) -> loFreq.
// Used for the live theremin-style sounds in the tantrum flows.
int distanceToFreq(long d, int loFreq, int hiFreq) {
  d = constrain(d, (long)MOVEMENT_CM, (long)SENSE_FAR_CM);
  return map(d, MOVEMENT_CM, SENSE_FAR_CM, hiFreq, loFreq);
}

// Same idea but for brightness: close hand = bright, far = dim (0..255).
uint8_t distanceToBrightness(long d) {
  d = constrain(d, (long)MOVEMENT_CM, (long)SENSE_FAR_CM);
  return map(d, MOVEMENT_CM, SENSE_FAR_CM, 255, 40);
}
