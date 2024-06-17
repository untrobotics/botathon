# Dependencies
* python 3+
* [Bleak](https://github.com/hbldh/bleak/tree/master)
* [pygame](https://github.com/pygame/pygame)

# Usage

Name the peripheral "BotathonTeam23" and begin broadcasting before running the program.

Ensure Bluetooth is enabled on the device the script is running on.

The program expects a characteristic with the ID "19B10011-E8F2-537E-4F6C-D104768A1214". Ensure the peripheral has a characteristic with this ID and read/write permissions.

Please connect an Xbox controller to the computer to use.

# Platforms
* Windows 11

# To-do
* Test Windows 10
* Test MacOS
* Allow setting the team/peripheral name the scrcipt searches for
* Test that data is sent properly
