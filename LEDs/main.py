import ctypes
import neopixel
import board
import time
import multiprocessing

import atexit
from functions import StartIdle,StartTimer,StartShootOut,StartSuddenDeath,at_exit,deinit
from constants import STARTCOUNTDOWN, IDLELIGHTS


pipeFront, pipeBack = multiprocessing.Pipe()

if __name__=='__main__':
    atexit.register(at_exit)
    if STARTCOUNTDOWN:
        print("This timer will have a 3 second countdown before starting.\n")
    else:
        print("Timer will start as soon as user inputs...maybe idk\n")
    #idk if terminal will allow it (because it didn't work with C++)
    input("Press 'enter' to start\n")
    continueProgram = True
    while continueProgram:
            StartTimer(pipeFront,pipeBack)
            #ask if sudden death
            MatchOver = ""
            while True:
                MatchOver = input("\nIs the match over? Enter 0 to reset the timer, 1 to start sudden death, or 2 to turn off all LEDs and terminate the program.\nSudden death will start with a countdown.\n")
                if MatchOver[:1]=='0' or MatchOver[:1]=='1' or MatchOver[:1]=='2':
                    break
            if MatchOver[:1]=='2': #terminate program
                continueProgram=False

            elif MatchOver[:1]=='1': #sudden death
                StartSuddenDeath(pipeFront,pipeBack)
                while True:
                    MatchOver = input("\nIs the match over? Enter 0 to reset the timer, 1 to start Shoot-Out, or 2 to turn off all LEDs and terminate the program.\n")
                    if MatchOver[:1]=='0' or MatchOver[:1]=='1' or MatchOver[:1]=='2':
                        break
                if MatchOver[:1] == '1':
                    StartShootOut(pipeFront,pipeBack)
                    if IDLELIGHTS:
                        StartIdle(pipeFront,pipeBack)
                    else:
                        input("\nPress Enter to start the timer.\n")
                elif MatchOver[:1] == '2':
                    continueProgram = False #should i change this to break
                else:
                    if IDLELIGHTS:
                        StartIdle(pipeFront, pipeBack)
                    else:
                        input("\nPress Enter to start the timer.\n")

            else:
                if IDLELIGHTS:
                    StartIdle(pipeFront, pipeBack)
                else:
                    input("\nPress Enter to start the timer.\n")

    deinit()