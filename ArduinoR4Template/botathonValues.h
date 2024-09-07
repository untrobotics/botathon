// Some of these values need to be changed to get your bot working!

// Change TEAM_NAME to use your team number
// You do not need to change TEAM_NO, but it may be helpful to show your team number there
#define TEAM_NAME "BotathonTeam23"
#define TEAM_NO   23  //used for the LED matrix

// uncomment this to have the LED matrix show debugging messages
// #define DEBUG_MATRIX

// Enum for button values
// See: https://www.learn-c.org/en/Bitmasks or README on bitmasks
enum class Inputs{  // doesn't include triggers because triggers are similar to joysticks
  AButton             = 1,      //Bit 1
  BButton             = 2,      //Bit 2
  XButton             = 4,      //Bit 3
  YButton             = 8,      //Bit 4
  BumperLeft          = 16,     //Bit 5
  BumperRight         = 32,     //Bit 6
  LeftCenterButton    = 64,     //Bit 7
  RightCenterButton   = 128,    //Bit 8
  
  LeftJoystickButton  = 256,    //Bit 9
  RightJoystickButton = 512,    //Bit 10
  XBoxButton          = 1024,   //Bit 11
  CenterCenterButton  = 2048,   //Bit 12
  DPadUp              = 4096,   //Bit 13
  DPadRight            = 8192,   //Bit 14
  DPadDown            = 16384,  //Bit 15
  DPadLeft           = 32768,  //Bit 16
};

// This function tells you if a button is pressed. Pass the current value to c and the button you want to check to input
//Example usages:
// buttonPressed(currentValue, Input::AButton);
bool buttonPressed(unsigned int c,Inputs input);


// These don't need to be changed
#define CHARACTERISTIC_ID "19B10011-E8F2-537E-4F6C-D104768A1214"
#define BAUD_RATE 115200

// This also doesn't technically need to be changed
#define SERVICE_ID "1ae49b08-b750-4ef7-afd8-5395763c0da6"

