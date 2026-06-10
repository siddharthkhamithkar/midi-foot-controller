/*
This is an arduino program to simulate footswitch presses
and trigger specific MIDI controls across your DAW.

Short Press (< 1 second)
Long Press (>= 1 second)

Written by yours truly, Siddharth Khamithkar
*/

const int NUM_SWITCHES = 6;

// Footswitch pins
const int fsPins[NUM_SWITCHES] = {2, 3, 4, 5, 6, 7};

// Track previous state for edge detection
bool lastState[NUM_SWITCHES];

// Hold detection
unsigned long pressStartTime[NUM_SWITCHES];
bool holdTriggered[NUM_SWITCHES];

void sendNote(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

void sendShortPress(int switchNumber) {

  switch (switchNumber) {

    case 0:
      sendNote(0x9A, 0x00, 0x45); // C-1
      break;

    case 1:
      sendNote(0x9A, 0x01, 0x45); // C#-1
      break;

    case 2:
      sendNote(0x9A, 0x02, 0x45); // D-1
      break;

    case 3:
      sendNote(0x9A, 0x03, 0x45); // D#-1
      break;

    case 4:
      sendNote(0x9A, 0x04, 0x45); // E-1
      break;

    case 5:
      sendNote(0x9A, 0x05, 0x45); // F-1
      break;
  }
}

void sendLongPress(int switchNumber) {

  switch (switchNumber) {

    case 0:
      sendNote(0x9A, 0x10, 0x45);
      break;

    case 1:
      sendNote(0x9A, 0x11, 0x45);
      break;

    case 2:
      sendNote(0x9A, 0x12, 0x45);
      break;

    case 3:
      sendNote(0x9A, 0x13, 0x45);
      break;

    case 4:
      sendNote(0x9A, 0x14, 0x45);
      break;

    case 5:
      sendNote(0x9A, 0x15, 0x45);
      break;
  }
}

void setup() {

  Serial.begin(31250);

  for (int i = 0; i < NUM_SWITCHES; i++) {

    pinMode(fsPins[i], INPUT_PULLUP);

    lastState[i] = HIGH;
    holdTriggered[i] = false;
    pressStartTime[i] = 0;
  }
}

void loop() {

  for (int i = 0; i < NUM_SWITCHES; i++) {

    bool currentState = digitalRead(fsPins[i]);

    // Switch pressed
    if (currentState == LOW && lastState[i] == HIGH) {

      pressStartTime[i] = millis();
      holdTriggered[i] = false;

      delay(20); // debounce
    }

    // Switch held
    if (currentState == LOW && !holdTriggered[i]) {

      if (millis() - pressStartTime[i] >= 200) {

        sendLongPress(i);
        holdTriggered[i] = true;
      }
    }

    // Switch released
    if (currentState == HIGH && lastState[i] == LOW) {

      if (!holdTriggered[i]) {

        sendShortPress(i);
      }

      delay(20); // debounce
    }

    lastState[i] = currentState;
  }
}