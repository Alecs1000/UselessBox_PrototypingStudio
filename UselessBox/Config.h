#pragma once

//Config file to check connection variable and fast setting for the Box

// ---- Pins ----
const uint8_t PIN_SWITCH  = 2;   // the lever. wired to GND, uses the internal pullup
const uint8_t PIN_BUZZER  = 3;   // passive buzzer
const uint8_t PIN_LED     = 4;   // NeoPixel data-in
const uint8_t PIN_MOTOR_1 = 6;   // "king" servo - the guy who pops out of the box
const uint8_t PIN_MOTOR_2 = 5;   // arm servo - the one that shoves the lever back off
const uint8_t PIN_TRIG    = 10;  // ultrasonic trigger
const uint8_t PIN_ECHO    = 11;  // ultrasonic echo

// ---- NeoPixel ----
const uint8_t NUM_PIXELS = 1;   //The box only cotains one RGB LED, But i need to specify the number for how the Neopixel works

// ---- Behaviour tuning ----
const int  ANIM_DELAY      = 300;   // base pause between animation steps (ms)
const int  TANTRUM_DELAY   = 600;   // slower pause between tantrum wiggle moves (more dramatic)
const long WAIT_TIMEOUT_MS = 8000;  // how long the king waits outside before giving up
const long TANTRUM_WATCH_MS = 6000; // how long a tantrum plays its sensor mind-game before snapping anyway
const int  MOVEMENT_CM     = 5;     // closer than this = someone's reaching for the switch
const int  SENSE_FAR_CM    = 40;    // past this = "no hand near", the far end of the sensor mapping

// ---- "Peekaboo" interaction(no lever flip needed) ----
// While idle, if a hand creeps toward the sensor the king rises on his own.
const int  PEEK_START_CM   = 35;    // hand nearer than this while idle = start the peekaboo, king begins to rise
const int  PEEK_TOP_CM     = 10;    // hand this close = king fully up + winning jingle
const long PEEK_TIMEOUT_MS = 5000;  // if the hand wanders off mid-game, give up and reset

// ---- IMU shake detection (MMA7660) ----
// Sum of the X/Y/Z jump (raw counts, ~21 per g) between two reads. A still box
const int  SHAKE_THRESHOLD = 28;

// ---- Telling a knock from a shake ----
// When a jolt wakes the box we watch a short window and decide what it was.
// A knock = sharp taps with quiet gaps; a shake = continuous motion.
const long KNOCK_GAP_MS      = 600;   // this much silence ends a knock sequence
const long JOLT_MAX_MS       = 2500;  // safety cap on the classify window
const int  SHAKE_ENERGY_LIMIT = 220;  // total jerk over the window above this = it's a shake
// (duty cycle - fraction of "hot" samples - above 35% also counts as a shake)
const int  KNOCK_MIN_TAPS    = 2;     // fewer taps than this = stray jolt, ignored

// ---- Escalating rage (shake) ----
const int  RAGE_MAX         = 4;      // angriest tier; tier 4 = full meltdown (flips the lever)
const long SHAKE_RECHECK_MS = 350;    // window to check if they're STILL shaking between bursts

// ---- LED mood colors (R, G, B) ----
#define COLOR_RED     strip.Color(255,   0,   0)
#define COLOR_ORANGE  strip.Color(255,  90,   0)
#define COLOR_YELLOW  strip.Color(255, 200,   0)
#define COLOR_PURPLE  strip.Color(160,   0, 200)
#define COLOR_GREEN   strip.Color(  0, 255,  40)   // "you win" green for the peekaboo top
#define COLOR_CYAN    strip.Color(  0, 180, 200)   // curious "who's there?" for knock-knock

// ---- Servo angles ----
const int ARM_IN    = 120;   // motor_1 tucked away inside the box
const int ARM_OUT   = 25;  // motor_1 popped out
const int FLIP_REST = 180;    // motor_2 resting
const int FLIP_PUSH = 45;  // motor_2 pushing the lever off

// ---- Flipper "tease" angles (v2) ----
// Lower angle = closer to the lever. The arm taunts here BEFORE the real slam,
// so these must stop SHORT of actually tripping the switch (tune on the box):
//   FLIP_HOVER - poised just over the lever, menacing but idle
//   FLIP_FAKE  - the deepest a bluff-jab lunges; must NOT reach the trip point
const int FLIP_HOVER = 88;    // hover - pushed forward so the hand clears the lid, visible
const int FLIP_FAKE  = 55;    // fake-jab depth - hair off the lever threshold (still > FLIP_PUSH 45)

// ---- Flipper tease timing (v2) ----
// Deliberate, not frantic: the arm shows itself over the lid and means it.
const int  TEASE_JABS     = 3;     // fewer, more decisive bluff jabs before the boom
const int  TEASE_FAST_MS  = 90;    // gap between jabs once wound up (still measured, not manic)
const int  TEASE_SLOW_MS  = 260;   // gap between the first, slow, looming jabs
