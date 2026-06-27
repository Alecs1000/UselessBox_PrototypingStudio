#pragma once.

#define NOTE_E4  330
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_C6  1047
#define NOTE_E6  1319
#define NOTE_D6  1175
#define NOTE_A6  1760
#define NOTE_C7  2093
#define NOTE_D7  2349
#define NOTE_E7  2637   // up in the buzzer's loud zone
#define REST     0      // silence

// One step of a tune: a note and how long to hold it (ms).
struct Note {
  int freq;
  int duration;
};

// ---- beat 1: "loot chest opens" fanfare - rising arpeggio with a triumphant top note ----
// Think of the little jingle when you crack open a chest in a game.
const Note BEAT_1[] = {
  { NOTE_G4, 90  },
  { NOTE_C5, 90  },
  { NOTE_E5, 90  },
  { NOTE_G5, 90  },
  { NOTE_C6, 110 },
  { REST,    40  },
  { NOTE_G5, 90  },
  { NOTE_C6, 320 },   // held triumphant note
};
const int BEAT_1_LEN = sizeof(BEAT_1) / sizeof(BEAT_1[0]);

// ---- RAGE: flow A (red) - harsh fast alarm, like he's furious ----
// Two high notes slammed back and forth, then a screaming top note.
const Note BEAT_RAGE[] = {
  { NOTE_E7, 80 }, { NOTE_C7, 80 },
  { NOTE_E7, 80 }, { NOTE_C7, 80 },
  { NOTE_E7, 80 }, { NOTE_C7, 80 },
  { NOTE_D7, 280 },   // long angry blast
};
const int BEAT_RAGE_LEN = sizeof(BEAT_RAGE) / sizeof(BEAT_RAGE[0]);

// ---- TAUNT: flow B (orange) - playful mocking "nyah-nyah" seesaw ----
const Note BEAT_TAUNT[] = {
  { NOTE_A6, 150 }, { NOTE_E6, 150 },
  { NOTE_A6, 150 }, { NOTE_E6, 240 },
};
const int BEAT_TAUNT_LEN = sizeof(BEAT_TAUNT) / sizeof(BEAT_TAUNT[0]);

// ---- WIN: peekaboo top - bright rising "you did it!" fanfare (level-up feel) ----
const Note BEAT_WIN[] = {
  { NOTE_E5, 100 },
  { NOTE_G5, 100 },
  { NOTE_C6, 100 },
  { NOTE_E6, 120 },
  { REST,    40  },
  { NOTE_G5, 90  },
  { NOTE_E6, 420 },   // held triumphant top
};
const int BEAT_WIN_LEN = sizeof(BEAT_WIN) / sizeof(BEAT_WIN[0]);

// ---- beat 2: flow C (purple) - grumpy descending grumble (raised up so it's audible) ----
const Note BEAT_2[] = {
  { NOTE_C6, 120 },
  { NOTE_A5, 120 },
  { NOTE_E5, 180 },
};
const int BEAT_2_LEN = sizeof(BEAT_2) / sizeof(BEAT_2[0]);
