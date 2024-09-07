# Dependencies
* python 3+
* [SimpleBLE](https://github.com/OpenBluetoothToolbox/SimpleBLE)
* [SDL2](https://wiki.libsdl.org/SDL2/Installation) and [PySDL2](https://github.com/py-sdl/py-sdl2/tree/master)
  * *You can also use the PySDL2-dll package instead of installing/compiling SDL2 yourself*
  * *If you're not using PySDL2-dll, you may need to set an environment variable `PYSDL2_DLL_PATH` to the directory holding the SDL2 dll (or equivalent).*
# Usage

Name the peripheral "BotathonTeam{number}" (where {number} is anything) and begin broadcasting.

Ensure Bluetooth is enabled on the device the script is running on. After starting the script and when prompted, enter the same number as above.

The program expects a characteristic with the ID "19B10011-E8F2-537E-4F6C-D104768A1214". Ensure the peripheral has a characteristic with this ID and read/write permissions.

Please connect an Xbox controller to the computer to use.

# Platforms
* Windows 10/11

# To-do
* Test MacOS (10.15+ because SimpleBLE does not support versions earlier than Catalina)

# Quick SDL2 run-down
The event types used in this script are
 * 1536 (Joystick and triggers, these are currently ignored)
 * 1538 (D-Pad)
 * 1539 (Button down)
 * 1540 (Button up)
 * 1541 (Controller connected))

Button numbering and the corresponding button on an Xbox controller
0. A Button
1.  B Button
2.  X Button
3.  Y Button
4.  Left Bumper
5.  Right Bumper
6.  Left center Button (i.e., select)
7.  Right Center Button (i.e., start)
8.  Left Joystick in
9.  Right Joystick in
10. Xbox Button
11. Share button (on Xbox Series X|S controllers) 

The D-Pad is measured via a bitmask. Starting from the least significant bit, it is
* Up (0001 or 1)
* Right (0010 or 2)
* Down (0100 or 4)
* Left (1000 or 8)