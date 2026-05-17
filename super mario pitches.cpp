/*
  AROMIN, Arley Bentley S.
  BAET 1 - A

  Super Mario Bros. - Complete Theme
  original melody transcription from various community sources.
  Structure and code organization done with the help of modern tools
*/

#include "pitches.h"
#define MELODY_PIN 11

int melody[] = {
  // Main theme - Part 1
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0, 0,
  NOTE_G6, 0, 0, 0,

  // Main theme - Part 2
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  // Main theme - Part 2 repeat
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  // Underworld / fast section
  NOTE_C7, NOTE_C7, 0, NOTE_C7,
  0, NOTE_C7, NOTE_D7, 0,
  NOTE_E7, NOTE_C7, 0, NOTE_A6,
  NOTE_A6, NOTE_B6, 0, 0,

  NOTE_D7, 0, NOTE_F7, NOTE_D7,
  NOTE_D7, NOTE_C7, NOTE_E7, NOTE_C7,
  NOTE_C7, NOTE_A6, 0, 0,

  NOTE_D7, 0, NOTE_F7, NOTE_D7,
  NOTE_FS7, NOTE_G7, 0, NOTE_E7,
  NOTE_C7, NOTE_D7, 0, 0,

  // Star theme / Bridge
  NOTE_E7, NOTE_C7, NOTE_G6, 0,
  NOTE_GS6, NOTE_A6, NOTE_F7, NOTE_A7,
  NOTE_D7, 0, 0, 0,

  NOTE_E7, NOTE_C7, NOTE_G6, 0,
  NOTE_GS6, NOTE_A6, NOTE_F7, NOTE_A7,
  NOTE_C8, 0, 0, 0,

  // Main theme reprise - Part 1
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  // Ending
  NOTE_C7, 0, NOTE_G6, 0,
  NOTE_E6, 0, NOTE_A6, NOTE_B6,
  NOTE_A6, NOTE_GS6, NOTE_AS6, NOTE_GS6,
  NOTE_G6, NOTE_D7, NOTE_G7
};

int durations[] = {
  // Main theme - Part 1
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  // Main theme - Part 2
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  // Main theme - Part 2 repeat
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  // Underworld / fast section
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  // Star theme / Bridge
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  // Main theme reprise
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,
  8, 8, 8, 8,

  // Ending
  4, 4, 4, 4,
  4, 4, 8, 8,
  8, 8, 8, 8,
  4, 4, 2
};

void setup() {
  pinMode(MELODY_PIN, OUTPUT);
}

void loop() {
  int size = min(
    sizeof(melody) / sizeof(melody[0]),
    sizeof(durations) / sizeof(durations[0])
  );
  for (int note = 0; note < size; note++) {
    int duration = 1000 / durations[note];
    tone(MELODY_PIN, melody[note], duration);
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    noTone(MELODY_PIN);
  }
  delay(2000);
}
