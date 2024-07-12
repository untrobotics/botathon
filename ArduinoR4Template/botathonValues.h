//don't need to change
#define CHARACTERISTIC_ID "19B10011-E8F2-537E-4F6C-D104768A1214"
#define BAUD_RATE 115200

// change team name as needed
#define TEAM_NAME "BotathonTeam23"
#define TEAM_NO   23  //used for the LED matrix
// change service ID to be unique
#define SERVICE_ID "1ae49b08-b750-4ef7-afd8-5395763c0da6"

// uncomment to have the LED matrix show debugging messages
// #define DEBUG_MATRIX

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
  DpadDown            = 8192,   //Bit 14
  DPadLeft            = 16384,  //Bit 15
  DPadRight           = 32768,  //Bit 16
};

bool buttonPressed(unsigned int c,Inputs input);