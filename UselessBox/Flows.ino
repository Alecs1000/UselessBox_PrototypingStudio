// ===== The box's behaviour "flows" =====
// One function per mode so I can read, tweak and A/B test each on its own.
//
//   flowStandard - flip 1, 2, 3: the normal routine
//   flowA / B / C - the three random tantrums for the later pokes
//
// They all share retreat() at the end (king back in, lever flipped off, lights out).

#include "Config.h"

// ---------------------------------------------------------------------------
// FLOW 1 - standard mode
// King's arm pops out (armed), lights + buzzer do their usual thing, then he
// watches the ultrasonic sensor. If anything comes within range inside 8s, he
// arms the second motor to flip the lever and both motors drop back down.
// (If nothing shows up in 8s he gives up and retreats anyway.)
// ---------------------------------------------------------------------------
void flowStandard() {
  kingPopOut();              // motor_1 armed / out FIRST
  delay(ANIM_DELAY);         // let him finish standing up before we start watching

  setColor(COLOR_YELLOW);    // usual light...
  startBlink(ANIM_DELAY);
  buzzerBeat();              // ...and usual loot-chest jingle

  // Only NOW does the sensor start looking. Watch for up to 8s, or bail early the
  // moment something comes close.
  long startTime = millis();
  long lastPrint = 0;
  while (true) {
    updateBlink();                                  // keep the light blinking while waiting

    // throttle the debug print so we don't flood the monitor
    if (millis() - lastPrint > 300) {
      lastPrint = millis();
      Serial.print(F("    dist: ")); Serial.print(readDistanceCm()); Serial.println(F(" cm"));
    }

    if (sensorSeesMovement()) { Serial.println(F("    -> movement seen")); break; }
    if (millis() - startTime >= WAIT_TIMEOUT_MS) { Serial.println(F("    -> 8s timeout")); break; }
  }

  retreat();                 // motor_2 flips the lever, everything comes back down
}

// ---------------------------------------------------------------------------
// FLOW A  (flowchart "case 0" - red)  ->  "THE AMBUSH"
// He bursts out furious and DARES you to reach for the lever. The closer your
// hand gets, the faster the red flashes and the higher the siren screams - then
// the instant you cross the line he slams the lever off. Punishes the approach.
// (If you chicken out and never come close, he slams anyway after a few seconds.)
// ---------------------------------------------------------------------------
void flowA() {
  setColor(COLOR_RED);
  buzzerRage();                  // opening battle cry

  flipperHover();                // arm rears up over the lever - "go on, touch it"

  long start = millis();
  long lastJab = 0;
  while (true) {
    long d = readDistanceCm();

    // closer hand -> faster blink + higher siren. Live tone follows your hand.
    long blink = map(constrain(d, (long)MOVEMENT_CM, (long)SENSE_FAR_CM),
                     MOVEMENT_CM, SENSE_FAR_CM, 60, 400);  // close=frantic, far=calm
    startBlink(blink);
    updateBlink();
    tone(PIN_BUZZER, distanceToFreq(d, NOTE_C6, NOTE_E7));

    // Dare-jabs: the closer your hand, the more often the arm fakes a flip at you.
    // (The fake-jab beep briefly hijacks the siren - that stutter sells the taunt.)
    long jabGap = map(constrain(d, (long)MOVEMENT_CM, (long)SENSE_FAR_CM),
                      MOVEMENT_CM, SENSE_FAR_CM, 250, 1200);
    if (millis() - lastJab > jabGap) {
      lastJab = millis();
      Serial.println(F("    [A] dare-jab"));
      flipperFakeJab(0);         // bluff lunge, no extra beep (siren resumes after)
    }

    Serial.print(F("    [A] dist ")); Serial.print(d); Serial.print(F(" cm  blink ")); Serial.println(blink);

    if (d < MOVEMENT_CM) { Serial.println(F("    [A] you came too close -> SLAM")); break; }
    if (millis() - start >= TANTRUM_WATCH_MS) { Serial.println(F("    [A] timeout -> SLAM anyway")); break; }
  }
  noTone(PIN_BUZZER);

  endTantrum();                  // the real slam to FLIP_PUSH
}

/* ---------------------------------------------------------------------------
 FLOW B  (flowchart "case 1" - orange)  ->  "THE PSYCH-OUT"
 The arm winds up and hovers right over the lever, taunting. But he refuses to
 flip while your hand is watching - reach in and he yanks back to rest with a
 mocking "nyah-nyah". He only commits and flips once you give up and pull away.
--------------------------------------------------------------------------- */
void flowB() {
  setColor(COLOR_ORANGE);
  startBlink(ANIM_DELAY);

  flipperHover();                // wind up: hover the arm just over the lever
  delay(TANTRUM_DELAY);

  long start = millis();
  long lastTwitch = 0;
  while (true) {
    updateBlink();
    long d = readDistanceCm();
    Serial.print(F("    [B] dist ")); Serial.println(d);

    // Idle menace: when no hand is near, the arm drums its fingers + sneaks a
    // bluff jab now and then. Keeps it alive instead of just hovering still.
    if (d >= MOVEMENT_CM && millis() - lastTwitch > 700) {
      lastTwitch = millis();
      flipperTwitch();
      if (random(0, 2)) flipperFakeJab(NOTE_C6);   // half the time, a teasing fake
    }

    if (d < MOVEMENT_CM) {       // caught you reaching -> psych! pull back and mock
      Serial.println(F("    [B] hand spotted -> PSYCH, pull back"));
      flipper.write(FLIP_REST);
      startBlink(ANIM_DELAY - 200);  // fuming fast blink
      buzzerTaunt();
      flipperHover();               // creep back over the lever again
      delay(TANTRUM_DELAY);
      start = millis();             // reset the patience timer - the game continues
    }

    // hand is away (or never came) for long enough -> strike while you're not ready
    if (millis() - start >= TANTRUM_WATCH_MS) {
      Serial.println(F("    [B] coast clear -> wind-up + FLIP"));
      flipperTeaseBurst(TEASE_JABS, NOTE_C6);   // frantic accelerating bluffs...
      break;                                    // ...then the boom (endTantrum)
    }
  }

  endTantrum();
}

// ---------------------------------------------------------------------------
// FLOW C  (flowchart "case 2" - purple)  ->  "THE STAREDOWN"
// Dead calm and creepy. A low theremin hum and a slow purple glow that both
// rise as your hand approaches - he's locking eyes with you. The moment you
// actually commit (get right up to the lever) he grumbles and flips it.
// ---------------------------------------------------------------------------
void flowC() {
  setColor(COLOR_PURPLE);
  stopBlink();                   // no blinking - a steady, unsettling stare

  long start = millis();
  long lastTwitch = 0;
  while (true) {
    long d = readDistanceCm();

    setBrightnessNow(distanceToBrightness(d));        // glow swells as you near
    tone(PIN_BUZZER, distanceToFreq(d, NOTE_E4, NOTE_C6));  // low hum rises in pitch

    // The menace: the arm creeps from rest toward a hover over the lever as your
    // hand closes in - the nearer you get, the more it's poised to strike.
    int crept = map(constrain(d, (long)MOVEMENT_CM, (long)SENSE_FAR_CM),
                    MOVEMENT_CM, SENSE_FAR_CM, FLIP_HOVER, FLIP_REST);
    flipper.write(crept);

    // Every so often a single, deliberate twitch breaks the stillness - creepy,
    // not frantic. Just enough to remind you it's alive and impatient.
    if (millis() - lastTwitch > 1300) { lastTwitch = millis(); flipperTwitch(); }

    Serial.print(F("    [C] dist ")); Serial.print(d); Serial.print(F("  arm ")); Serial.println(crept);

    if (d < MOVEMENT_CM) { Serial.println(F("    [C] you committed -> grumble + flip")); break; }
    if (millis() - start >= TANTRUM_WATCH_MS) { Serial.println(F("    [C] timeout -> flip")); break; }
  }
  noTone(PIN_BUZZER);
  buzzerBeat2();                 // grumpy parting grumble

  endTantrum();                  // dead-still stare snaps into the boom slam
}

// ---------------------------------------------------------------------------
// PEEKABOO  -  ambient interaction, no lever flip involved
// While the box is just sitting idle, if someone slowly creeps a hand toward the
// sensor the king rises on his own, degree by degree, following the distance.
// Reach the top and he celebrates: winning jingle + a slow, happy green blink.
// But push your luck and come back a second time and he turns on you: red blink,
// ducks back down, and the flipper sweeps the lever back off.
// ---------------------------------------------------------------------------
void flowPeekaboo() {
  Serial.println(F("  -> peekaboo (slow approach)"));

  // --- Stage 1: rise with the approaching hand ---
  // distance PEEK_START_CM..PEEK_TOP_CM maps to king angle ARM_IN..ARM_OUT.
  long start = millis();
  bool reachedTop = false;
  while (millis() - start < PEEK_TIMEOUT_MS) {
    long d = readDistanceCm();
    int angle = map(constrain(d, (long)PEEK_TOP_CM, (long)PEEK_START_CM),
                    PEEK_TOP_CM, PEEK_START_CM, ARM_OUT, ARM_IN);
    king.write(angle);
    Serial.print(F("    [peek] dist ")); Serial.print(d); Serial.print(F(" -> king ")); Serial.println(angle);

    if (d <= PEEK_TOP_CM) { reachedTop = true; break; }
    delay(30);
  }

  if (!reachedTop) {            // hand wandered off - king sits back down, nothing happens
    Serial.println(F("    [peek] hand left -> abort"));
    kingTuckIn();
    delay(ANIM_DELAY);
    return;
  }

  // --- Top reached: you win ---
  king.write(ARM_OUT);
  setColor(COLOR_GREEN);
  startBlink(600);             // slow, calm, happy blink
  buzzerWin();                 // victory fanfare

  // Wait for the hand to pull away first, so a lingering hand isn't read as the
  // "second" approach. Keep the slow green blink going meanwhile.
  while (readDistanceCm() < PEEK_START_CM) updateBlink();
  Serial.println(F("    [peek] hand backed off - waiting to see if you push your luck"));

  // --- Stage 2: did they come back? ---
  start = millis();
  bool cameBack = false;
  while (millis() - start < PEEK_TIMEOUT_MS) {
    updateBlink();
    if (readDistanceCm() < PEEK_TOP_CM) { cameBack = true; break; }
  }

  if (!cameBack) {             // they quit while ahead - king ducks down peacefully
    Serial.println(F("    [peek] left in peace -> gentle retreat"));
    kingTuckIn();
    delay(ANIM_DELAY);
    ledOff();
    stopBlink();
    return;
  }

  // --- Pushed their luck: he turns on them ---
  Serial.println(F("    [peek] came back -> ANGRY: down + flip"));
  setColor(COLOR_RED);
  startBlink(100);             // fast angry red
  buzzerRage();
  kingTuckIn();                // king ducks down
  delay(ANIM_DELAY);
  flipLeverUntilOff(4000);     // flipper sweeps the lever back off (gesture even if it's already off)
  delay(ANIM_DELAY);
  flipperRest();
  delay(ANIM_DELAY);
  ledOff();
  stopBlink();
}

/* ---------------------------------------------------------------------------
 JOLT classifier  -  something moved the box. Knock or shake?
 We're called the instant the first spike is felt. Watch a short window:
  - sharp taps with quiet gaps (low duty cycle) -> it's a KNOCK -> flowKnock
  - continuous, energetic motion (high duty/energy) -> it's a SHAKE -> escalate
 --------------------------------------------------------------------------- */
void flowJolt() {
  int  knocks  = 1;        // the spike that woke us counts as the first tap
  int  energy  = 0;
  int  hot     = 0, total = 0;
  bool prevHot = true;     // we got here because it was just "hot"
  long lastActivity = millis();
  long startT       = millis();

  // Keep watching until things go quiet for KNOCK_GAP_MS (or we hit the safety cap).
  while (millis() - lastActivity < KNOCK_GAP_MS && millis() - startT < JOLT_MAX_MS) {
    int  j     = shakeJerk();
    bool isHot = j > SHAKE_THRESHOLD;
    total++;
    if (isHot) { hot++; energy += j; lastActivity = millis(); }
    if (isHot && !prevHot) knocks++;   // a fresh rising edge = another tap
    prevHot = isHot;
    delay(12);
  }

  float duty = total ? (float)hot / total : 0;
  Serial.print(F("  jolt -> knocks ")); Serial.print(knocks);
  Serial.print(F(", energy ")); Serial.print(energy);
  Serial.print(F(", duty ")); Serial.println(duty);

  if (duty > 0.35 || energy > SHAKE_ENERGY_LIMIT) flowShakenEscalate(energy);
  else                                            flowKnock(knocks);
}

// ---------------------------------------------------------------------------
// KNOCK-KNOCK  -  someone taps the box politely
// "Who's there?" The king peeks up curious (cyan), then knocks back the same
// number of taps with the flipper, a little beep on each. Playful, not angry.
// ---------------------------------------------------------------------------
void flowKnock(int knocks) {
  // A single stray jolt is not a knock - ignore it so he never reacts on his own.
  if (knocks < KNOCK_MIN_TAPS) {
    Serial.print(F("  -> single stray jolt (")); Serial.print(knocks);
    Serial.println(F(" tap) - ignoring"));
    return;
  }

  knocks = constrain(knocks, 1, 5);
  Serial.print(F("  -> knock-knock! replying with ")); Serial.print(knocks); Serial.println(F(" taps"));

  setColor(COLOR_CYAN);
  startBlink(400);                       // slow, curious

  king.write((ARM_IN + ARM_OUT) / 2);    // peek up halfway... who's there?
  delay(ANIM_DELAY);
  king.write(ARM_OUT);                   // ...all the way out, listening
  delay(ANIM_DELAY);

  // knock back: tap the flipper inward the same number of times, beep each tap
  for (int i = 0; i < knocks; i++) {
    flipper.write(FLIP_REST - 30);       // tap in
    tone(PIN_BUZZER, NOTE_C6, 80);
    delay(140);
    flipper.write(FLIP_REST);            // back to rest
    delay(160);
    updateBlink();
  }
  noTone(PIN_BUZZER);

  delay(ANIM_DELAY);
  king.write(ARM_IN);                    // satisfied, ducks back in
  delay(ANIM_DELAY);
  ledOff();
  stopBlink();
}

// ---------------------------------------------------------------------------
// SHAKEN (escalating)  -  someone manhandles the box
// The king hates it. He reacts at rage tier 1, and if you KEEP shaking he climbs
// the tiers: faster red blink, higher/harsher tone, bigger tremble. Reach the top
// tier and he has a full meltdown and flips the lever himself.
// ---------------------------------------------------------------------------
void flowShakenEscalate(int seedEnergy) {
  Serial.print(F("  -> SHAKEN! escalating (seed energy ")); Serial.print(seedEnergy); Serial.println(F(")"));

  setColor(COLOR_RED);
  int rage = 1;

  do {
    Serial.print(F("    rage tier ")); Serial.println(rage);

    startBlink(map(rage, 1, RAGE_MAX, 140, 45));   // angrier = faster flashing
    int pitch = map(rage, 1, RAGE_MAX, NOTE_C6, NOTE_E7);
    int amp   = 5 + rage * 3;                       // angrier = bigger tremble
    int reps  = 3 + rage * 2;                       // angrier = longer fit

    for (int i = 0; i < reps; i++) {
      tone(PIN_BUZZER, pitch + (i % 2) * 120);      // harsh two-tone warble
      king.write(constrain(ARM_OUT - amp, 0, 180));
      delay(115);                                   // slower, weightier sway (was 60 = frantic)
      king.write(constrain(ARM_OUT + amp, 0, 180));
      delay(115);
      updateBlink();
    }
    noTone(PIN_BUZZER);
    king.write(ARM_OUT);

    if (rage >= RAGE_MAX) {                          // top tier -> full meltdown
      Serial.println(F("    MELTDOWN -> flipping the lever himself"));
      buzzerRage();
      flipLeverUntilOff(3000);
      flipperRest();
      break;
    }
    rage++;
  } while (stillShaking());                          // keep climbing while they keep shaking

  // calm down
  noTone(PIN_BUZZER);
  delay(ANIM_DELAY);
  king.write(ARM_IN);                                // storms back down inside
  delay(ANIM_DELAY);
  ledOff();
  stopBlink();
}

// ---------------------------------------------------------------------------
// Shared endings
// ---------------------------------------------------------------------------

// How the tantrum flows (A/B/C) wrap up: shove the lever off, count the poke, reset.
void endTantrum() {
  flipLeverUntilOff(4000); // keep hammering (up to 4s) until the switch reads OFF
  delay(ANIM_DELAY);
  flipperRest();           // motor_2 back to rest
  delay(ANIM_DELAY);
  ledOff();
  stopBlink();
}

// How the standard flow wraps up: king ducks in, arm flips the lever, lights out.
void retreat() {
  kingTuckIn();            // motor_1 back in
  flipLeverUntilOff(4000); // keep hammering (up to 4s) until the switch reads OFF
  delay(ANIM_DELAY);
  ledOff();
  stopBlink();
  flipperRest();           // motor_2 back to rest
  delay(ANIM_DELAY);
}
