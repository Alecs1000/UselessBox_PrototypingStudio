// ===== Servo helpers =====
// Two micro servos:
//   king    (motor_1) - the character that pops out of the box and ducks back in
//   flipper (motor_2) - the little arm that shoves the lever back off

#include "Config.h"

void kingPopOut() { king.write(ARM_OUT); }  
void kingTuckIn() { king.write(ARM_IN);  }   

void flipLeverOff() { flipper.write(FLIP_PUSH); }  // - knocks the lever off
void flipperRest()  { flipper.write(FLIP_REST); }  // - back to resting

// Keep hammering the lever until the switch actually reads OFF (or we give up).
// which helps a weak servo nudge a stiff lever.

bool flipLeverUntilOff(long timeoutMs) {
  long start = millis();
  int  attempt = 0;

  while (digitalRead(PIN_SWITCH) == LOW) {   // LOW = lever still ON
    attempt++;
    flipper.write(FLIP_REST);                // wind back...
    delay(120);
    flipper.write(FLIP_PUSH);                // ...and snap to the lever with momentum
    delay(250);

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
