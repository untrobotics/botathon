import board
import configparser
import neopixel

boardNum = board.D18

configFilePath = "/home/pi/Documents/led.cfg"

# edit the config file to do this stuff
config = configparser.ConfigParser()
config.read(configFilePath)
# default values for config if it can't find the cfg file
config['DEFAULT'] = {
    "StartCountdown": "1",
    "YellowSwitch": "1",
    "AutoTerm": "1",
    "IdleLights": "0",
    "StartTime": "180",
    "LowTime": "30",
    "SDTime": "90",
    "Countdown": "FFFFFF",
    "TimerNormal": "00FF00",
    "LowTimeColor": "00FCFC",
    "GoalScored": "0000FF",
    "TimeUp": "FF0000",
    "IdleOne": "FFFFFF",
    "IdleTwo": "00853E", #this is UNT Green (according to https://identityguide.unt.edu/create-our-look/color)
    "ShootOutOne": "FFD700",
    "ShootOutTwo":"FFFFFF",
    "SuddenDeath":"FF0000",
    "LEDCount": "300"
}

# color configs
temp = config.get("COLORS", "Countdown")
COLOR_COUNTDOWN = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))

temp = config.get("COLORS", "TimerNormal")
COLOR_TIMER = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))

temp = config.get("COLORS", "LowTimeColor")
COLOR_LOWTIME = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))

temp = config.get("COLORS", "GoalScored")
COLOR_GOALSCORED = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))

temp = config.get("COLORS", "TimeUp")
COLOR_TIMEUP = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))

temp = config.get("COLORS", "IdleOne")
COLOR_IDLEONE = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))

temp = config.get("COLORS", "IdleTwo")
COLOR_IDLETWO = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))

temp = config.get("COLORS", "ShootOutOne")
COLOR_SOONE = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))
temp = config.get("COLORS", "ShootOutTwo")
COLOR_SOTWO = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))


temp = config.get("COLORS", "SuddenDeath")
COLOR_SD = (int(temp[:2], 16), int(temp[2:4], 16), int(temp[4:6], 16))

# behavior configs
STARTCOUNTDOWN = config.getboolean("BEHAVIOR", "StartCountdown")
YELLOWSWITCH = config.getint("BEHAVIOR", "YellowSwitch")
AUTOTERM = config.getint("BEHAVIOR", "AutoTerm")
IDLELIGHTS = config.getboolean("BEHAVIOR", "IdleLights")

# time configs
TIMERTIME = float(config.getint("TIMER", "StartTime"))
LOWTIME = config.getint("TIMER", "LowTime")
SDTIME = config.getint("TIMER", "SDTime")

pxlCnt = config.getint("MISC", "LEDCount")
pixels = neopixel.NeoPixel(boardNum, pxlCnt, auto_write=False)