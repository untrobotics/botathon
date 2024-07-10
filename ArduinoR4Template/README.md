# WIP

# Installation

Ensure  `botathonValues.h` is in the same Sketchbook directory as `main.ino`.

Ensure the following libraries are installed in Arduino IDE:
* ArduinoBLE (by Arduino)
* ArduinoGraphics (by Arduino)

# Usage

Add your code between lines *insert line numbers here* in `main.ino`. 

If you don't want the LED matrix on the Arduino to show debug messages, comment out line 6, where it says ``#define DEBUG_MATRIX``.

## Getting controller button states

The function `buttonPressed` will tell you if a button is pressed down (or if it's not). Parameters are
1. `unsigned int c` - Pass `controlCharacteristic.value` here.
2. `Inputs input` - The button(s) you want to see is/are pressed down.

The function will return `true` if all `Inputs` passed in parameter 2 are pressed down. Otherwise, it will return false.

### Example usage

Assuming the A button is pressed down,

```c++
buttonPressed(controlCharacteristic.value, Inputs::AButton);
```

will return `true`.

An equivalent function call is

```c++
buttonPressed(controlCharacteristic.value, (Inputs) 1);
```

# To-do
* Add a dictionary or macro so users don't have to use enum
* Add basic motor control

# Bugs
* Windows requires pairing to use BLE. Current code will not run otherwise.
