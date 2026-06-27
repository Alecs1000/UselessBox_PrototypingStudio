//Passive buzzer main function

#include "Config.h"
#include "Tones.h"

// Play a melody (array of Notes) one note after another.
void playBeat(const Note melody[], int length) {
  for (int i = 0; i < length; i++) {
    if (melody[i].freq == REST) {
      noTone(PIN_BUZZER);
    } else {
      tone(PIN_BUZZER, melody[i].freq, melody[i].duration);
    }
    delay(melody[i].duration + 30);  // small gap so notes don't blur together
  }
  noTone(PIN_BUZZER);
}

void buzzerBeat()  { playBeat(BEAT_1, BEAT_1_LEN); }          // flow 1: loot-chest jingle
void buzzerRage()  { playBeat(BEAT_RAGE, BEAT_RAGE_LEN); }    // flow A: furious alarm
void buzzerTaunt() { playBeat(BEAT_TAUNT, BEAT_TAUNT_LEN); }  // flow B: mocking seesaw
void buzzerBeat2() { playBeat(BEAT_2, BEAT_2_LEN); }          // flow C: grumpy grumble
void buzzerWin()   { playBeat(BEAT_WIN, BEAT_WIN_LEN); }      // peekaboo top: victory fanfare
