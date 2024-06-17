import asyncio
from bleak import BleakScanner, BleakClient
import pygame
async def main():
    device = None
    while device == None:
        print("Scanning for 5 seconds...\n")
        devices = await BleakScanner.discover()
        print("Scan complete.")
        for d in devices:
            if d.name == 'BotathonTeam23':
                print('exiting scan')
                device = d
                break
        if device == None:
            print("Device not found.\n")
    print("Device found")

    async with BleakClient(device) as client:
        while True:
            event = pygame.event.wait()
            event_type = event.type
            value = await client.read_gatt_char("19B10011-E8F2-537E-4F6C-D104768A1214")

            if event_type == pygame.QUIT:   # supposedly handles SIGINT
                break
            if event_type >= 1536 or event_type <= 1540:
                key = ""
                if event_type == 1536:  # Input is an axis
                    # key = axisToString[eventDict['axis']]
                    # value = eventDict['value']
                    continue
                elif event_type == 1537:
                    continue
                elif event_type == 1538:  # Input is D-Pad
                    # value = {'x': event.value[0], 'y': event.value[1]}
                    set_bits = 0
                    if event.value[0] == 1:
                        set_bits = 128  # 1000 0000
                    elif event.value[0] == -1:
                        set_bits = 64   # 0100 0000
                    if event.value[1] == 1:
                        set_bits |= 16  # 0001 0000
                    elif event.value[1] == -1:
                        set_bits |= 32  # 0010 0000
                    value[1] = (value[1] & 0b00001111   # value[1] & 15
                                ) | set_bits   # resets bits 5-8 and then sets them
                    print(set_bits)
                else:  # Input is a button
                    # key = buttonToString[eventDict['button']]
                    # value = event_type == 1539
                    button = event.button
                    byte_index = int(button > 7)
                    if byte_index:
                        button -= 8
                    set_bit = (1 << button)
                    value[byte_index] = (value[byte_index] & (~set_bit))    # reset bitmask
                    if event_type == 1539:  # if the button is down, set bitmask
                        value[byte_index] |= set_bit
                    print(button,byte_index,set_bit)

                print(event)
                # List of event types (as far as I'm aware, this numbering is constant)
                # Axes input:    1536
                # Button Down:   1539        True
                # Button Up:     1540        False
                # D-Pad:         1538
                # there's also the gamepad being connected and mic/headphones (but no one cares about that)

                # list of buttons/axes and their corresponding xbox input:
                # NOTE: D-Pad comes with a value in an ordered pair representing x,y
                # Axes 1,0:  Left joystick, x,y
                # Axes 3,2:  Right joystick, x,y
                # Axis 4:    Left Trigger, positive is pushed in
                # Axis 5:    Right Trigger, same as Axis 4 (Left Trigger)

                # Button 0:  A Button
                # Button 1:  B Button
                # Button 2:  X Button
                # Button 3:  Y Button
                # Button 4:  Left Bumper
                # Button 5:  Right Bumper

                # The three center buttons (excluding the xbox)
                # Button 6:  Left center Button
                # Button 7:  Right Center Button
                # Button 11: Center Center Button

                # Button 8:  Left Joystick Button
                # Button 9:  Right Joystick Button
                # Button 10: XBox Button
                print("Input successfully sent.")
            else:
                print(event)

joysticks = []
doStuff = True

pygame.init()
print("Initializing controllers...")
joystickCount = pygame.joystick.get_count()
for i in range(0, joystickCount):
    joysticks.append(pygame.joystick.Joystick(i))
    joysticks[-1].init()
print(f"Initialized {joystickCount} controllers.")
try:
    from bleak.backends.winrt.util import allow_sta
    # tell Bleak we are using a graphical user interface that has been properly
    # configured to work with asyncio
    allow_sta()
except ImportError:
    # other OSes and older versions of Bleak will raise ImportError which we
    # can safely ignore
    pass
asyncio.run(main())
