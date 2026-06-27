//Imu main function (doubt cause the hardware imu is not in the best condition)

#include "MMA7660.h"
#include "Config.h"

// Last reading we saw, so we can measure how much the box jerked since then.
int8_t imuPrevX = 0, imuPrevY = 0, imuPrevZ = 0;

// Power up the accelerometer and grab a baseline so the first shake-check is honest.
void imuBegin() {
  imu.init();                                    // also brings up I2C (Wire)
  imu.getXYZ(&imuPrevX, &imuPrevY, &imuPrevZ);   // prime the "previous" sample
}

// How hard the box moved since the last call (the "jerk"): sum of the X/Y/Z jump.
// A still box barely changes; a knock spikes briefly; a shake stays high.
int shakeJerk() {
  int8_t x, y, z;
  imu.getXYZ(&x, &y, &z);

  int jerk = abs(x - imuPrevX) + abs(y - imuPrevY) + abs(z - imuPrevZ);
  imuPrevX = x; imuPrevY = y; imuPrevZ = z;
  return jerk;
}

// True if the box was just jolted harder than SHAKE_THRESHOLD.
bool sensorShaken() {
  return shakeJerk() > SHAKE_THRESHOLD;
}

// Watch a short window and report whether the box is STILL being moved.
// Used between rage bursts to decide if the king should keep escalating.
bool stillShaking() {
  long t = millis();
  while (millis() - t < SHAKE_RECHECK_MS) {
    if (shakeJerk() > SHAKE_THRESHOLD) return true;
    delay(15);
  }
  return false;
}
