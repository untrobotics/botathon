# WIP

# Installation

Ensure all of the files in this folder are in the same Sketchbook directory as `BotathonArduinoBLE.ino`.

Ensure the following libraries are installed in Arduino IDE:
* ArduinoBLE (by Arduino)
* ArduinoGraphics (by Arduino)

# Usage

Add your code between lines 89 and 100 in `BotathonArduinoBLE.ino`. 

If you don't want the LED matrix on the Arduino to show debug messages, comment out line 9 in `botathonValues.h`, where it says ``#define DEBUG_MATRIX``.

## Getting controller button states

The function `buttonPressed` will tell you if a button is pressed down (or if it's not). Parameters are
1. `unsigned int c` - Pass `controlCharacteristic.value` here.
2. `Inputs input` - The button(s) you want to see is/are pressed down.

The function will return `true` if all `Inputs` passed in parameter 2 are pressed down. Otherwise, it will return false.

### Example usage

Assuming the A button is pressed down,

```cpp
buttonPressed(controlCharacteristic.value, Inputs::AButton);
```

will return `true`.

An equivalent function call is

```cpp
buttonPressed(controlCharacteristic.value, (Inputs) 1);
```

If you want to see if multiple buttons are pressed down,
```cpp
buttonPressed(controlCharacteristic.value,Inputs::AButton | Inputs::BButton);
```
this code will only return `true` if both the A and B buttons are pressed down. This is not equivalent to separately checking if the A 