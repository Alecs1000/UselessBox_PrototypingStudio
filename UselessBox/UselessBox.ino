// ===== Useless Box =====
// You flip the lever, the "king" pops out of the box, looks at you, and an arm
// shoves the lever back off. The more you keep poking him, the grumpier he gets.

#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include "MMA7660.h"
#include "Config.h"
#include "Tones.h"  //TONES 

Servo king;     // motor_1 - the character that pops out
Servo flipper;  // motor_2 - the arm that flips the lever back off
Adafruit_NeoPixel strip(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);
MMA7660 imu;    // 3-axis accelerometer on the I2C bus (SDA/SCL)

// What woke the box up out of idle - decides which interaction runs.
enum WakeReason { WAKE_POKE, WAKE_APPROACH, WAKE_JOLT };

// How many times he's been poked this round:
//   0..2  -> he just pops out and looks around
//   3..4  -> he throws a random little tantrum
//   5     -> he's had enough, reset and start over
int flip = 0;

// --- non-blocking LED blink state (lets the light blink while we wait on the sensor) ---
bool     ledBlinking   = false;
long     ledBlinkDelay = 0;
long     lastBlink     = 0;
bool     ledOn         = false;
uint32_t ledColor      = 0;

void setup() {
  Serial.begin(9600);   // open Serial Monitor at 9600 to watch what the box is doing

  // Servos start in their resting position.
  king.attach(PIN_MOTOR_1);
  flipper.attach(PIN_MOTOR_2);
  king.write(ARM_IN);        // arm in
  flipper.write(FLIP_REST);

  // LED off to begin with.
  strip.begin();
  ledOff();

  // Buzzer + ultrasonic sensor pins.
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  // Lever on the internal pullup -> reads LOW when it's flipped on.
  pinMode(PIN_SWITCH, INPUT_PULLUP);

  imuBegin();                  // wake the accelerometer + grab a baseline reading

  randomSeed(analogRead(A0));  // so the tantrum pick is actually random each run
}

void loop() {
  // Wait for something to happen. Two things wake the box right now:
  //   - a genuine lever poke          -> the normal flip flows below
  //   - a knock or a shake (IMU jolt) -> knock-knock reply, or angry shake
  // (Idle proximity / peekaboo is parked - it fought with the IMU. To bring it
  //  back, return WAKE_APPROACH from waitForEvent() and re-add the dispatch line:
  //    if (wake == WAKE_APPROACH) { flowPeekaboo(); return; } )
  WakeReason wake = waitForEvent();
  if (wake == WAKE_JOLT) { flowJolt(); return; }  // doesn't count as a poke - flip stays put

  // This counts as one real poke.
  flip++;
  Serial.print(F("Poke #")); Serial.println(flip);

  if (flip <= 3) {
    Serial.println(F("  -> flow 1 (standard)"));
    flowStandard();          // flow 1: pop out, watch the sensor, flip lever, retreat
  } else {
    // pokes 4 and 5 -> one of the three tantrum flows, picked at random.
    // Keeping them separate so I can test A / B / C on the box and see what feels best.
    int pick = random(0, 3);   // 0, 1 or 2
    switch (pick) {
      case 0: Serial.println(F("  -> flow A (red jab)"));    flowA(); break;
      case 1: Serial.println(F("  -> flow B (orange wiggle)")); flowB(); break;
      case 2: Serial.println(F("  -> flow C (purple beat)"));   flowC(); break;
    }
  }

  // After 5 pokes the king gives up for this round and we start fresh.
  if (flip >= 5) {
    flip = 0;
  }
}

// Wait for the box to be woken out of idle. Returns WHAT woke it:
//   WAKE_POKE     -> the lever was flipped OFF then back ON (a real poke)
//   WAKE_APPROACH -> a hand crept toward the sensor (peekaboo)
//   WAKE_JOLT     -> the box was knocked or shaken (IMU) - flowJolt sorts out which
//
// A poke only counts once the lever has gone fully OFF and then been flipped back
// ON - so the box's own arm knocking the lever off can never be mistaken for a poke.
// INPUT_PULLUP: pin reads LOW when the switch is ON (closed to GND), HIGH when OFF.
WakeReason waitForEvent() {
  while (digitalRead(PIN_SWITCH) == LOW) {   // lever still ON - wait for it to go OFF
    updateBlink();
  }
  delay(20);                                 // debounce the release

  // Lever is OFF now: the box is idle. Watch for a wake-up event.
  while (digitalRead(PIN_SWITCH) == HIGH) {  // lever OFF
    updateBlink();
    if (sensorShaken()) return WAKE_JOLT;    // knocked or shaken
    // NOTE: idle proximity (peekaboo) is parked - it clashed with the IMU jolt
    // wake-up (a reaching hand kept firing it). flowPeekaboo() is kept below for a
    // possible reentry; see loop() for how to switch it back on.
  }
  delay(20);                                 // debounce the press
  return WAKE_POKE;                          // lever flipped ON -> real poke
}
