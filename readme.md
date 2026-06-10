# MIDI Foot Controller — Arduino + macOS Bridge

This repository provides a compact footswitch-to-MIDI solution. An Arduino sketch reads multiple footswitches and sends raw 3-byte MIDI packets over the Arduino's USB serial connection. A small macOS menubar bridge (`middleware.py`) listens on the serial port and forwards decoded messages to the system MIDI bus (IAC Driver), allowing DAWs and other apps to receive the notes.

Key components
- `jumpedFootswitch/jumpedFootswitch.ino`: Arduino sketch that reads multiple footswitches, distinguishes short vs long presses, and sends 3-byte MIDI messages (status, note, velocity) over USB serial.
- `middleware/middleware.py`: Python menubar app (uses `rumps`) that detects the Arduino serial port and macOS IAC port, decodes incoming serial bytes into `mido.Message` objects, and forwards them to the selected MIDI output.
- `Arduino Midi Bridge.spec`: PyInstaller spec for packaging the bridge as a standalone macOS app.

How it works
- The Arduino writes raw 3-byte MIDI packets to the USB serial connection at MIDI baud (31250).
- The Python bridge opens the USB serial port, decodes messages, builds `mido` messages, and sends them to the macOS IAC port so other apps receive the notes.

Wiring (physical connections)
- Pushbutton footswitch wiring (one per switch):
	- One side of the switch -> Arduino digital pin (example: pins 2,3,4,5,6,7).
	- Other side of the switch -> Arduino GND.
	- The sketch uses `INPUT_PULLUP`, so no external resistors are required.
	- Ensure a common ground between all switches and the Arduino.
- Suggested pin mapping (matches sketch):
	- Switch 1: D2
	- Switch 2: D3
	- Switch 3: D4
	- Switch 4: D5
	- Switch 5: D6
	- Switch 6: D7
- Notes:
	- Use momentary (normally open) footswitches.
	- This project uses the Arduino's USB serial (onboard USB). It does not use a separate 5-pin MIDI DIN out.

Basic setup & run
1. Upload `jumpedFootswitch.ino` to your Arduino and connect the Arduino to your Mac via USB.
2. Enable the macOS IAC Driver in Audio MIDI Setup (if you want other apps to receive the notes).
3. Create and activate a Python virtual environment, then install dependencies:

```bash
python -m venv .venv
source .venv/bin/activate
pip install mido python-rtmidi rumps pyserial
```

4. Run the bridge:

```bash
python middleware/middleware.py
```

Or build a standalone macOS app with PyInstaller using `Arduino Midi Bridge.spec`.

Compatibility and notes
- Tested on macOS (IAC Driver). The bridge expects the Arduino on a USB serial port and the IAC bus for forwarding messages.
- Adjust serial and MIDI settings for other platforms or different Arduino hardware.

Author: Siddharth Khamithkar
