#include <SparkFunSX1509.h>
#include <Wire.h> 
#include <frequencyToNote.h>
#include <MIDIUSB.h>
#include <MIDIUSB_Defs.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

/*
 * Made by Orion Culbertson
 * 
 * This runs a 4x4 button MIDI controller with 6 side
 * buttons which each load their own set of MIDI notes.
 */

const int NUM_ROWS = 4;
const int NUM_COLS = 4;
const byte CHANNEL = 1;
const byte VELOCITY = 127;

long deounceTime = 8;
long lastDebounceTime = 0;

byte rowPins[4] = {4, 5, 6, 7};
byte colPins[4] = {8, 9, 10, 16};

int ledPins[4][4] = {
  {0, 1, 2, 3},
  {4, 5, 6, 7},
  {8, 9, 10, 11},
  {12, 13, 14, 15}
};

SX1509 matrixLeds;
SX1509 sideButtons;

int sideButtonPins[] = {0, 1, 2, 6, 7, 8};
int sideLEDPins[] = {3, 4, 5, 9, 10, 11};

boolean currentState[4][4] = {
  {false, false, false, false},
  {false, false, false, false},
  {false, false, false, false},
  {false, false, false, false}
};

boolean previousState[4][4] = {
  {false, false, false, false},
  {false, false, false, false},
  {false, false, false, false},
  {false, false, false, false}
};

byte noteSetOne[4][4] = {
  {16, 17, 18, 19},
  {12, 13, 14, 15},
  {8, 9, 10, 11},
  {4, 5, 6, 7}
};

byte noteSetTwo[4][4] = {
  {32, 33, 34, 35},
  {28, 29, 30, 31},
  {24, 25, 26, 27},
  {20, 21, 22, 23}
};

byte noteSetThree[4][4] = {
  {48, 49, 50, 51},
  {44, 45, 46, 47},
  {40, 41, 42, 43},
  {36, 37, 38, 39},
};

byte noteSetFour[4][4] = {
  {64, 65, 66, 67},
  {60, 61, 62, 63},
  {56, 57, 58, 59},
  {52, 53, 54, 55}
};

byte noteSetFive[4][4] = {
  {80, 81, 82, 83},
  {76, 77, 78, 79},
  {72, 73, 74, 75},
  {68, 69, 70, 71}
};

byte noteSetSix[4][4] = {
  {96, 97, 98, 99},
  {92, 93, 94, 95},
  {88, 89, 90, 91},
  {84, 85, 86, 87}
};

byte (*currentNoteSet)[4][4];

void setup() {
  Serial.begin(115200);

  //Start the I/O expanders
  matrixLeds.begin(0x3E);
  sideButtons.begin(0x3F);
  
  //Button Matrix LEDs
  for (int i = 0; i < 16; i++) {
    matrixLeds.pinMode(i, OUTPUT);
    matrixLeds.digitalWrite(i, LOW);
  }

  //Side Buttons
  for (int i = 0; i < 6; i++) {
    sideButtons.pinMode(sideButtonPins[i], INPUT_PULLUP);
  }

  //Side LEDs
  for (int i = 0; i < 6; i++) {
    sideButtons.pinMode(sideLEDPins[i], OUTPUT);
    sideButtons.digitalWrite(sideLEDPins[i], LOW);
  }
  
  
  //Button Matrix
  for(int x = 0; x < NUM_ROWS; x++) {
      pinMode(rowPins[x], INPUT);
  }

  for (int x = 0; x < NUM_COLS; x++) {
      pinMode(colPins[x], INPUT_PULLUP);
  }

  //Set the current note set
  currentNoteSet = &noteSetOne;
  sideButtons.digitalWrite(sideLEDPins[0], HIGH);
  sideButtons.digitalWrite(sideLEDPins[1], HIGH);
  sideButtons.digitalWrite(sideLEDPins[2], HIGH);
  sideButtons.digitalWrite(sideLEDPins[3], HIGH);
  sideButtons.digitalWrite(sideLEDPins[4], HIGH);
  sideButtons.digitalWrite(sideLEDPins[5], HIGH);
  
}

/*
 * This is the loop that runs everything.
 */
void loop() {
  checkSideButtons();
  readMatrix();
}

/*
 * This checks what buttons are and aren't pressed relative
 * to the last scan of buttons in the matrix. It calls 
 * checkDifferences to perform actions based on the recorded 
 * state.
 * 
 * Code modified and originally from https://www.baldengineer.com/arduino-keyboard-matrix-tutorial.html
 */
void readMatrix() {
  for (int row = 0; row < NUM_ROWS; row++) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], LOW);

    for (int col = 0; col < NUM_COLS; col++) {
      pinMode(colPins[col], INPUT_PULLUP);
      currentState[row][col] = !digitalRead(colPins[col]);
      checkDifferences(row, col);
      pinMode(colPins[col], INPUT);
    }
    pinMode(rowPins[row], INPUT);
  }
}

/*
 * This only performs an action if a button's scan is different
 * than its previous scan. If it's different, then it will either
 * turn a note on or off depending. Then it records the current 
 * state to the previous state.
 */
void checkDifferences(int row, int col) {
  if (currentState[row][col] != previousState[row][col]) {
        if (currentState[row][col] == true) {
          matrixLeds.digitalWrite(ledPins[row][col], HIGH);
          noteOn(CHANNEL, (*currentNoteSet)[row][col], VELOCITY);
          MidiUSB.flush();
        } else {
          matrixLeds.digitalWrite(ledPins[row][col], LOW);
          noteOn(CHANNEL, (*currentNoteSet)[row][col], 0);
          MidiUSB.flush();
        }
  }

  previousState[row][col] = currentState[row][col];
}

/* 
 * This checks to see if any of the 6 side buttons are being pressed.
 * If a side button is pressed, the note bank will change and the button
 * that is pressed will have its LED lit.
 */
void checkSideButtons() {
  if (sideButtons.digitalRead(sideButtonPins[0]) == LOW) {
    stopCurrentNotes();
    currentNoteSet = &noteSetOne;
    toggleSideLEDs(0);
    startCurrentNotes();
  } else if (sideButtons.digitalRead(sideButtonPins[1]) == LOW) {
    stopCurrentNotes();
    currentNoteSet = &noteSetTwo;
    toggleSideLEDs(1);
    startCurrentNotes();
  } else if (sideButtons.digitalRead(sideButtonPins[2]) == LOW) {
    stopCurrentNotes();
    currentNoteSet = &noteSetThree;
    toggleSideLEDs(2);
    startCurrentNotes();
  } else if (sideButtons.digitalRead(sideButtonPins[3]) == LOW) {
    stopCurrentNotes();
    currentNoteSet = &noteSetFour;
    toggleSideLEDs(3);
    startCurrentNotes();
  } else if (sideButtons.digitalRead(sideButtonPins[4]) == LOW) {
    stopCurrentNotes();
    currentNoteSet = &noteSetFive;
    toggleSideLEDs(4);
    startCurrentNotes();
  } else if (sideButtons.digitalRead(sideButtonPins[5]) == LOW) {
    stopCurrentNotes();
    currentNoteSet = &noteSetSix;
    toggleSideLEDs(5);
    startCurrentNotes();
  }
}

/*
 * This goes through the side button LEDs on the box, turns 
 * off the inactive ones, and turns on the active one.
 */
void toggleSideLEDs(int button) {
  for (int i = 0; i < 6; i++) {
    if (i == button) {
      sideButtons.digitalWrite(sideLEDPins[i], HIGH);
    } else {
      sideButtons.digitalWrite(sideLEDPins[i], LOW);
    }
  }
}

/*
 * This stops any active notes
 */
void stopCurrentNotes() {
  for (int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < NUM_COLS; j++) {
      if (currentState[i][j] == true) {
        noteOff(CHANNEL, (*currentNoteSet)[i][j], 0);
      }
    }
  }
  MidiUSB.flush();
}

/*
 * This starts any active notes
 */
void startCurrentNotes() {
  for (int i = 0; i < NUM_ROWS; i++) {
    for (int j = 0; j < NUM_COLS; j++) {
      if (currentState[i][j] == true) {
        noteOn(CHANNEL, (*currentNoteSet)[i][j], VELOCITY);
      }
    }
  }
  MidiUSB.flush();
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
