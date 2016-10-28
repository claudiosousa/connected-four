// Partition to play
typedef struct {
  int length;
  float* notes;
  int* durations;
} Sound;

Sound* current_sound = NULL;

byte next_note = 0;
unsigned long next_tone_duration = 0;

// Default volume (0-512)
int sound_duty_cycle = -1;
byte sound_gpio = D0;

// Partitions
float gameOverSoundNotes[] = {note_B3, note_F4, note_F4, note_F4, note_E4, note_D4, note_C4, note_E3, note_E3, note_C3};
int gameOverSoundDurations[] = {162, 5, 162, 162, 162, 5,   217, 5, 217, 5, 217, 5, 162, 5,   162, 162, 162, 5, 162, 5};
Sound gameOverSound = {.length = sizeof(gameOverSoundNotes) / sizeof(float), .notes = gameOverSoundNotes, .durations = gameOverSoundDurations};

float submitMoveSoundNotes[] = {300, 435, 631, 915, 1326, 1923};
int submitMoveSoundDurations[] = {60, 0, 60, 0, 60, 0, 60, 0, 60, 0, 60, 0};
Sound submitMoveSound = {.length = sizeof(submitMoveSoundNotes) / sizeof(float), .notes = submitMoveSoundNotes, .durations = submitMoveSoundDurations};

float startSoundNotes[] = {note_E5, note_G5, note_E6, note_C6, note_D6, note_G6};
int startSoundDurations[] = {130, 5, 130, 5, 130, 5, 130, 5, 130, 5, 130};
Sound startSound = {.length = sizeof(startSoundNotes) / sizeof(float), .notes = startSoundNotes, .durations = startSoundDurations};

float win_sound_notes[] = {
  note_G2,  note_C3,  note_E3,  note_G3,  note_C4,  note_E4,  note_G4, note_E4,  note_Ab2, note_C3,  note_Eb3, note_Ab3, note_C4,  note_Eb4,
  note_Ab4, note_Eb4, note_Bb2, note_D3,  note_F3,  note_Bb3, note_D4, note_F4,  note_Bb4, note_B4,  note_B4,  note_B4,  note_C5
};

int win_sound_duration[] = {
  130, 2, 130, 2, 130, 2, 130, 2, 130, 2, 130, 2, 433, 2, 433, 2, 130, 2,
  130, 2, 130, 2, 130, 2, 130, 2, 130, 5, 433, 2, 433, 2, 130, 2, 130, 2,
  130, 2, 130, 2, 130, 2, 130, 2, 433, 2, 130, 2, 130, 2, 130, 2, 650
};
Sound win_sound = {.length = sizeof(win_sound_notes) / sizeof(float), .notes = win_sound_notes, .durations = win_sound_duration};

void setup_sound() {
#ifdef DEBUG
  Serial.println("Sound setup up");
#endif
  sound_off();
}

void setvolume(int vol) {
  if (sound_duty_cycle == -1)  // the device got initialized
    play_start();
  sound_duty_cycle = vol;
}

void play_start() {
  current_sound = &startSound;
  sound_on();
}

void play_submitmove() {
  current_sound = &submitMoveSound;
  sound_on();
}

void play_gameover() {
  current_sound = &gameOverSound;
  sound_on();
}

void play_win() {
  current_sound = &win_sound;
  sound_on();
}

void sound_off() {
#ifdef DEBUG
  Serial.println("Sound off");
#endif
  current_sound = NULL;
  next_note = 0;
  next_tone_duration = 0;

  analogWrite(sound_gpio, 0);
}

void sound_on() {
#ifdef DEBUG
  Serial.println("Sound on");
#endif
  if (sound_duty_cycle > 1) {
    next_tone_duration = 0;
  }
}

void loop_sound() {
  if (current_sound == NULL)
    return;

  if (millis() < next_tone_duration)
    return;

  int f = 0;
  if (!(next_note % 2)) f = int(current_sound->notes[next_note / 2]);
#ifdef DEBUG
  Serial.print("play sound, freq : ");
  Serial.println(f);
#endif
  if (f != 0) {
    analogWriteFreq(f);
    analogWrite(sound_gpio, sound_duty_cycle);
  } else
    analogWrite(sound_gpio, 0);

  next_tone_duration = millis() + current_sound->durations[next_note];
  next_note++;
  if (next_note >= current_sound->length * 2) {
    sound_off();
  }
}
