// ===== Servo helpers =====
// Two micro servos:
//   king    (motor_1) - the character that pops out of the box and ducks back in
//   flipper (motor_2) - the little arm that shoves the lever back off

#include "Config.h"

void kingPopOut() { king.write(ARM_OUT); }  
void kingTuckIn() { king.write(ARM_IN);  }   

void flipLeverOff() { flipper.write(FLIP_PUSH); }  // - knocks the lever off
void flipperRest()  { flipper.write(FLIP_REST); }  // - back to resting

// ===== Flipper "tease" vocabulary (v2) =====
// Gives the arm an annoyed personality: it threatens the lever, bluffs, drums
// its fingers - all without committing - so the eventual slam lands as a "boom".
// All of these keep the LED blink alive (updateBlink) so the box never freezes.

// Poised just over the lever - the threat pose.
void flipperHover() { flipper.write(FLIP_HOVER); }

// A bluff: lunge toward the lever as if to flip it... then pull back. Never
// reaches the switch (stops at FLIP_FAKE). Deliberate, not twitchy - it pushes
// right out over the lid, HOLDS there so you see the threat, then withdraws.
void flipperFakeJab(int beepNote) {
  flipper.write(FLIP_FAKE);                 // lunge forward, out over the lid...
  if (beepNote) tone(PIN_BUZZER, beepNote, 90);
  delay(220);                              // ...and HOLD - let the threat land
  flipper.write(FLIP_HOVER);                // ...then withdraw, unhurried
  delay(200);
  noTone(PIN_BUZZER);
  updateBlink();
}

// A slow, deliberate sway at the hover - looming over the lever, not jittering.
void flipperTwitch() {
  flipper.write(FLIP_HOVER - 8);
  delay(130);
  flipper.write(FLIP_HOVER + 5);
  delay(130);
  flipper.write(FLIP_HOVER);
  updateBlink();
}

// A run of bluff jabs that ACCELERATE - winds the tension up toward the boom.
// Pitch climbs with each jab so the bluff sounds like it's losing patience.
void flipperTeaseBurst(int jabs, int startNote) {
  flipperHover();
  delay(120);
  for (int i = 0; i < jabs; i++) {
    flipperFakeJab(startNote + i * 40);
    delay(map(i, 0, jabs - 1, TEASE_SLOW_MS, TEASE_FAST_MS));  // lazy -> frantic
  }
}

// Keep hammering the lever until the switch actually reads OFF (or we give up).
// which helps a weak servo nudge a stiff lever.

bool flipLeverUntilOff(long timeoutMs) {
  long start = millis();
  int  attempt = 0;

  while (digitalRead(PIN_SWITCH) == LOW) {   // LOW = lever still ON
    attempt++;
    flipper.write(FLIP_HOVER);               // gentle partial wind-back (not a full violent cock)
    delay(260);
    flipper.write(FLIP_PUSH);                // ...then press into the lever, deliberate
    delay(420);

    Serial.print(F("    flip attempt ")); Serial.print(attempt);
    Serial.print(F(" -> switch "));
    Serial.println(digitalRead(PIN_SWITCH) == LOW ? "still ON" : "OFF");

    if (millis() - start > timeoutMs) {
      Serial.println(F("    !! gave up: lever still ON at full push -> likely STRENGTH/mechanical, not angle"));
      return false;
    }
  }

  Serial.println(F("    lever OFF confirmed"));
  return true;
}
