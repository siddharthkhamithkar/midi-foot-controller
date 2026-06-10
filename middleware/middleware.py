import os
import sys
import threading
import serial
import mido
import rumps

from queue import Queue, Empty
from serial.tools import list_ports


def resource_path(relative_path):
    # Resolve path for bundled resources (PyInstaller)
    try:
        base_path = sys._MEIPASS
    except AttributeError:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)


def note_name(note):

    names = [
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    ]

    # Convert MIDI note number to readable name
    octave = (note // 12) - 2

    return f"{names[note % 12]}{octave}"


class MidiBridgeApp(rumps.App):

    def __init__(self):

        # Initialize menubar app with icon
        super().__init__(
            "",
            icon=resource_path("menubar.png")
        )

        self.message_count = 0
        self.ui_queue = Queue()

        self.status_item = rumps.MenuItem(
            "🟡 Starting..."
        )

        self.arduino_item = rumps.MenuItem(
            "Arduino: Detecting..."
        )

        self.midi_item = rumps.MenuItem(
            "MIDI: Detecting..."
        )

        self.last_note_item = rumps.MenuItem(
            "Last Note: None"
        )

        self.count_item = rumps.MenuItem(
            "Messages: 0"
        )

        self.menu = [
            self.status_item,
            self.arduino_item,
            self.midi_item,
            self.last_note_item,
            self.count_item,
        ]

        self.bridge_thread = threading.Thread(
            target=self.run_bridge,
            daemon=True
        )

        self.bridge_thread.start()

    def find_arduino(self):

        # Look for Arduino by common HWID/manufacturer/device hints
        for port in list_ports.comports():

            if (
                "2341:0043" in port.hwid
                or "Arduino" in str(port.manufacturer)
                or "usbmodem" in port.device
            ):
                return port.device

        return None

    def find_iac(self):

        # Find macOS IAC driver by name
        for name in mido.get_output_names():

            if "IAC" in name:
                return name

        return None

    @rumps.timer(0.2)
    def update_ui(self, _):

        try:

            while True:

                event = self.ui_queue.get_nowait()

                event_type = event["type"]

                if event_type == "status":

                    self.status_item.title = event["value"]

                elif event_type == "arduino":

                    self.arduino_item.title = (
                        f"Arduino: {event['value']}"
                    )

                elif event_type == "midi":

                    self.midi_item.title = (
                        f"MIDI: {event['value']}"
                    )

                elif event_type == "message":

                    note = event["note"]

                    self.message_count += 1

                    note_text = note_name(note)

                    self.last_note_item.title = (
                        f"Last Note: {note_text}"
                    )

                    self.count_item.title = (
                        f"Messages: {self.message_count}"
                    )

                    # Live update in menu bar
                    self.title = note_text

        except Empty:
            pass

    def run_bridge(self):

        try:

            serial_port = self.find_arduino()

            if not serial_port:

                self.ui_queue.put({
                    "type": "status",
                    "value": "🔴 Arduino Not Found"
                })

                return

            self.ui_queue.put({
                "type": "arduino",
                "value": serial_port
            })

            midi_port = self.find_iac()

            if not midi_port:

                self.ui_queue.put({
                    "type": "status",
                    "value": "🔴 IAC Driver Not Found"
                })

                return

            self.ui_queue.put({
                "type": "midi",
                "value": midi_port
            })

            # Open serial and MIDI output
            ser = serial.Serial(
                serial_port,
                31250,
                timeout=1
            )

            midi_out = mido.open_output(
                midi_port
            )

            self.ui_queue.put({
                "type": "status",
                "value": "🟢 Connected"
            })

            while True:

                data = ser.read(3)

                if len(data) != 3:
                    continue

                status = data[0]
                note = data[1]
                velocity = data[2]

                if (status & 0xF0) == 0x90:

                    channel = status & 0x0F

                    # Build and forward a MIDI note_on message
                    msg = mido.Message(
                        "note_on",
                        channel=channel,
                        note=note,
                        velocity=velocity
                    )

                    midi_out.send(msg)

                    self.ui_queue.put({
                        "type": "message",
                        "note": note
                    })

        except Exception as e:

            print(f"ERROR: {e}")

            self.ui_queue.put({
                "type": "status",
                "value": f"🔴 Error"
            })


if __name__ == "__main__":
    MidiBridgeApp().run()
