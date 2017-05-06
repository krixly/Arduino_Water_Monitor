#include <Arduino.h>
#include "tones.h"

#ifndef CPLAY_BUZZER 
#define CPLAY_BUZZER 5
#endif

/*static void beep() {
  tone(CPLAY_BUZZER, 880, 20);
  }*/

void init_speaker() {
  pinMode(CPLAY_BUZZER, OUTPUT);
}

void speaker_tone(unsigned int frequency, unsigned long duration) { 
  tone(CPLAY_BUZZER, frequency, duration);
}

