# ESP-NOW Servo Controller

This project is an ESP-NOW based servo controller using an ESP32. The device can operate in two modes: `CENTRAL` and `REMOTE`. The `CENTRAL` mode receives activation messages and controls a servo motor, while the `REMOTE` mode sends activation messages to the central device.

## Hardware Requirements

- ESP32 board
- Servo motor
- Connecting wires

## Software Requirements

- Arduino IDE
- ESP32 board package installed in Arduino IDE
- ESP32Servo library

## Wiring

- Connect the servo signal pin to GPIO 3 of the ESP32.
- Connect the servo power and ground pins to the appropriate power and ground pins of the ESP32.
- The built-in LED of the ESP32-C3 is used for status indication.

## Setup

1. Clone this repository or download the `arduino-cat-tower-defense.ino` file.
2. Open the `arduino-cat-tower-defense.ino` file in the Arduino IDE.
3. Set the `DEVICE_ROLE` macro to either `"CENTRAL"` or `"REMOTE"` depending on the desired role of the device.
4. If using the `REMOTE` role, set the `centralMAC` array to the MAC address of the central device.
5. Upload the code to your ESP32 board.

## Usage

### CENTRAL Mode

In `CENTRAL` mode, the device will:

- Initialize ESP-NOW and register a callback for receiving data.
- Print its MAC address.
- Control the servo motor based on received activation messages.

### REMOTE Mode

In `REMOTE` mode, the device will:

- Initialize ESP-NOW and add the central device as a peer.
- Send an activation message to the central device and blink the LED.

## Functions

### `void onDataReceive(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len)`

Callback function for handling incoming data in `CENTRAL` mode.

### `void handleActivateMessage()`

Handles the activation message by moving the servo to the target position and back to the start position.

### `void setup()`

Initializes the device, sets up ESP-NOW, and configures the device based on its role.

### `void loop()`

Main loop function that handles sending activation messages in `REMOTE` mode.

### `void printMacAddress()`

Prints the MAC address of the device.

## License

This project is licensed under the MIT License.
