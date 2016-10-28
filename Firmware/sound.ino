/* Manage the sound */

//Partition to play
typedef struct {
  int length;
  float * notes;
  int * durations;
} Sound;

Sound * currentSound = NULL;

byte nextNote = 0;
unsigned long nextToneDuration;

// Hardware SETUP
int SoundDutyCycle = 512; // 0 = 0%, 512 = 50%, 1023 = 100%
int SoundGPIO = D0;


// Partitions
float gameOverSoundNotes[] = {  note_B3, note_F4, note_F4, note_F4, note_E4, note_D4,  note_C4, note_E3, note_E3, note_C3};
int gameOverSoundDurations[] = {162, 5,  162, 162,  162, 5,  217, 5, 217, 5, 217, 5, 162, 5, 162, 162, 162, 5, 162, 5};
Sound gameOverSound =  {.length = sizeof(gameOverSoundNotes) / sizeof(float), .notes = gameOverSoundNotes, .durations = gameOverSoundDurations};


float submitMoveSoundNotes[] =  { 300,   435,   631,   915,   1326,  1923};
int submitMoveSoundDurations[] = {60, 0, 60, 0, 60, 0, 60, 0, 60, 0, 60, 0};
Sound submitMoveSound =  {.length = sizeof(submitMoveSoundNotes) / sizeof(float), .notes = submitMoveSoundNotes, .durations = submitMoveSoundDurations};

float startSoundNotes[] =  { note_E5, note_G5, note_E6, note_C6, note_D6, note_G6};
int startSoundDurations[] = {130, 5,  130, 5,  130, 5,  130, 5,  130, 5,  130};
Sound startSound =  {.length = sizeof(startSoundNotes) / sizeof(float), .notes = startSoundNotes, .durations = startSoundDurations};

float winSoundNotes[] =  { note_G2, note_C3, note_E3, note_G3, note_C4, note_E4, note_G4, note_E4, note_Ab2, note_C3, note_Eb3, note_Ab3, note_C4, note_Eb4,
                           note_Ab4, note_Eb4, note_Bb2, note_D3, note_F3, note_Bb3, note_D4, note_F4, note_Bb4, note_B4, note_B4, note_B4, note_C5
                         };
int winSoundDurations[] = {130, 2,  130, 2,  130, 2,  130, 2,  130, 2,  130, 2,  433, 2,  433, 2,  130, 2,   130, 2,  130, 2,   130, 2,   130, 2,  130, 5,
                           433, 2,  433, 2,  130, 2,  130, 2,  130, 2,  130, 2,  130, 2,  130, 2,  433, 2,   130, 2,  130, 2,   130, 2,   650
                          };
Sound winSound =  {.length = sizeof(winSoundNotes) / sizeof(float), .notes = winSoundNotes, .durations = winSoundDurations};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup_sound() {
#ifdef DEBUG
  Serial.println("Sound setup up");
  pinMode(SoundGPIO, OUTPUT);
#endif
  sound_off();  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void soundvolume(int vol) { //Set up the volume level.
  SoundDutyCycle = vol;
}


// Which partition to play
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void playsstartsound() {
  currentSound = &startSound;
  soundon();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void playssubmitMovesound() {
  currentSound = &submitMoveSound;
  soundon();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void playsgameOversound() {
  currentSound = &gameOverSound;
  soundon();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void playswinsound() {
  currentSound = &winSound;
  soundon();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void sound_off() { //Cut off the sound
#ifdef DEBUG
  Serial.println("Sound off");
#endif
  return;
  currentSound = NULL;
  nextNote = 0;
  nextToneDuration = 0;

  analogWrite(SoundGPIO, 0) ;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void soundon() { //Put on the sound
#ifdef DEBUG
  Serial.println("Sound on");
#endif
  if ( SoundDutyCycle > 1 ) {                // Open audio amplificator and play sound only if sound volume > 1/1024 off duty cycle
    nextToneDuration = 0;//millis() + 1;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setvolume(String msg) { //Decode the volume level from the MQTT message
  soundvolume(msg.toInt());
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop_sound() { // Play partitions when neded

  if (currentSound == NULL)
    return;

  if (millis() < nextToneDuration)
    return;

  int f = 0;
  if (!(nextNote % 2))
    f = int(currentSound->notes[nextNote / 2]);
#ifdef DEBUG
  Serial.print("play sound, freq : ");
  Serial.println(f);
#endif
  if (f != 0) {
    analogWriteFreq(f);
    analogWrite(SoundGPIO, SoundDutyCycle) ;
  }
  else
    analogWrite(SoundGPIO, 0);

  nextToneDuration = millis() + currentSound->durations[nextNote];
  nextNote++;
  if (nextNote >= currentSound->length * 2) {
    sound_off();
  }
}

