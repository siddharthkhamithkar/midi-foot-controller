/*

This is an arduino program to simulate footswitch presses and trigger specific MIDI controls across your DAW.

I will probably revamp this entire thing once I am done with the hardware part of this project, because I'm using an Uno R3 and I will probably have to flash the entire thing.

Written by yours truly, Siddharth Khamithkar

*/



const int NUM_SWITCHES = 6; // define the total number of footswitches used

int fs[NUM_SWITCHES] = {0, 0, 0, 0, 0, 0};

void printMenu() {
  Serial.println();
  Serial.println("===== MIDI CONTROLLER TEST MENU =====");
  Serial.println("1 - Press Footswitch 1");
  Serial.println("2 - Press Footswitch 2");
  Serial.println("3 - Press Footswitch 3");
  Serial.println("4 - Press Footswitch 4");
  Serial.println("3 - Press Footswitch 5");
  Serial.println("4 - Press Footswitch 6");
  Serial.println("m - Show Menu");
  Serial.println("=====================================");
  Serial.println();
}

void pressFootSwitch(int switchNumber) {
  fs[switchNumber] = 1;

  String footSwitchPressedMessage = "Footswitch pressed: " + String(switchNumber + 1);
  Serial.println(footSwitchPressedMessage);

  fs[switchNumber] = 0;
}

void sendNote(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);

  String noteSentMessage = "Note sent: " + String(pitch);
  //Serial.println(noteSentMessage);
} 

void setup() {
  Serial.begin(31250); // setting the MIDI baud rate

  printMenu();
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();

    switch (command) {

      case '1':
        pressFootSwitch(0);
        sendNote(0x9A, 0x00, 0x45); //C -1
        break;

      case '2':
        pressFootSwitch(1);
        sendNote(0x9A, 0x01, 0x45); //C# -1
        break;

      case '3':
        pressFootSwitch(2);
        sendNote(0x9A, 0x02, 0x45); //D -1
        break;

      case '4':
        pressFootSwitch(3);
        sendNote(0x9A, 0x03, 0x45); //D# -1
        break;

      case '5':
        pressFootSwitch(4);
        sendNote(0x9A, 0x04, 0x45); //E -1
        break;

      case '6':
        pressFootSwitch(5);
        sendNote(0x9A, 0x05, 0x45); //F -1
        break;

      case 'm':
        printMenu();
        break;
    }
  }
}