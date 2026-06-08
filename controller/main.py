import ctypes
import simplepyble
from simplepyble import Peripheral
from sdl2 import SDL_Event, SDL_PollEvent, SDL_JoystickOpen, SDL_Init, SDL_INIT_JOYSTICK, SDL_INIT_EVENTS, \
    SDL_INIT_VIDEO

# Should be equal to whatever is set on the Arduino
SERVICE_UUID = '1ae49b08-b750-4ef7-afd8-5395763c0da6'
CHARACTERISTIC_UUID = '19b10011-e8f2-537e-4f6c-d104768a1214'


def get_adapter():
    """
    Gets the Bluetooth adapters on the computer. If there are multiple adapters, the user will be prompted in the
    terminal to select one.
    :return: The selected Bluetooth adapted or nothing if there are no adapters
    """
    # noinspection PyArgumentList
    adapters = simplepyble.Adapter.get_adapters()

    # if no adapters, output a message and exit the function
    if len(adapters) == 0:
        print("No adapters found")
        return

    # if there's only 1 adapter, we'll return that adapter
    if len(adapters) == 1:
        adptr = adapters[0]
    else:   # otherwise, there are multiple adapters, so we prompt the user to pick one
        # Ask the user to pick an adapter
        print("Please select an adapter:")
        # Display info on the available adapters
        for i, adptr in enumerate(adapters):
            print(f"{i}: {adptr.identifier()} [{adptr.address()}]")
        # Take input from the user
        choice = int(input("Enter choice: "))
        # set the selected adapter
        adptr = adapters[choice]

    # When the adapter starts scanning for Bluetooth devices, displays a message saying it's scanning
    adptr.set_callback_on_scan_start(lambda: print("Scanning for bluetooth devices."))
    # When the adapter stops scanning for Bluetooth devices, displays a message saying it's stopped
    adptr.set_callback_on_scan_stop(lambda: print("Scan complete."))
    # When the adapter finds a Bluetooth device, displays the device's information
    adptr.set_callback_on_scan_found(
        lambda device: print(f"Found {device.identifier()} [{device.address()}]"))
    return adptr


def get_peripheral(adptr, identifier) -> Peripheral:
    """
    Scans for and connects to a Bluetooth device
    :param adptr: The Bluetooth-capable adapter on the computer. You can get one by using get_adapter()
    :param identifier: The name of the Bluetooth device that we're trying to connect to
    :return: The Bluetooth device we've connected to
    """
    # device refers to the Bluetooth device we'll connect to
    # Set to None because we haven't found it or connected to it yet
    device = None
    while True:
        # Scan for 5 seconds... the program pauses until the 5 seconds are over
        adptr.scan_for(5000)

        # after the 5 seconds, get a list of all Bluetooth devices that were found in the 5 seconds
        peripherals = adptr.scan_get_results()

        # go through the list to see if any of them have the name we're looking for
        for p in peripherals:
            if p.identifier() == identifier:
                device = p
                break
        # stop looping if we found the device
        if device is not None:
            break

    # connect to the Bluetooth device. If you're on Windows, the device needs
        # to be in pairing mode or the program will crash (this is a Windows API *FEATURE* [bug])
    print(f"Connecting to: {device.identifier()} [{device.address()}]")
    device.connect()
    print("Connected to Arduino")
    # Return the device object
    return device


def dpad_input_to_str(old_val: int, new_val: int) -> str:
    """
    Gives a string saying which D-Pad buttons were pressed/stopped being pressed. This function is
    purely for outputting information to the terminal
    :param old_val: The previous D-Pad values as a bitmask. The order from the leftmost bit to right is
        Up, Right, Down, Left
    :param new_val: The current or new D-Pad values as a bitmask. Uses the same format as old-val
    :return: A string saying which D-Pad value changed and whether it was pressed down or released
    """
    change = old_val ^ new_val
    ret = ""
    ret += "top" if change == 1 else "right" if change == 2 else "bottom" if change == 4 else "left"
    return ret + " down" if change & new_val == change else " up"


# List purely for outputting information to the terminal. Basically tells you which button was pressed down.
# Use event.jbutton.button as the array index to get the string. This won't work with old_value or new_value
button_to_str = ["A button", "B button", "X button", "Y button", "Left bumper", "Right bumper", "Select", "Start",
                 "Left joystick", "Right joystick", "Xbox button", "Button 12"]

# Ask user for the team number
team_number = input("Enter your team number: ")
# Gets the Bluetooth adapter and connects to the Bluetooth device
adapter = get_adapter()
peripheral = get_peripheral(adapter, f"BotathonTeam{team_number}")

# initializes the controller library
print("Initializing SDL2")
SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS | SDL_INIT_VIDEO)
print("SDL2 initialized. Waiting for controller input...")

# event loop
while True:
    # gets the current list of inputs and reads through them
    event = SDL_Event()
    while SDL_PollEvent(ctypes.byref(event)):

        # This tells us what type of event occurred, such as a button being pressed or released or
        # if a new controller was connected to the computer
        event_type = event.type

        # get the current value from the Arduino so we can manipulate it
        old_value = int.from_bytes(peripheral.read(SERVICE_UUID, CHARACTERISTIC_UUID), "little")
        # new_value will equal old_value, except for having 1 bit changed
        new_value = old_value
        # 1541 means a controller was connected to the computer
        if event_type == 1541:
            SDL_JoystickOpen(0)
        elif event_type == 1538:  # Input is D-Pad
            print(f"D-pad {dpad_input_to_str(event.jbutton.state, old_value >> 12)}")

            # sets the proper bits for the bitmask representation of the controller state
            set_bits = event.jbutton.state << 12
            new_value = (new_value & 0b0000111111111111  # value & 4095 (b10)... resets the d-pad bits
                         ) | set_bits  # sets d-pad bits
            # print(set_bits)
        elif event_type == 1539 or event_type == 1540:  # Input is button up/down
            button = event.jbutton.button
            direction = " down" if event_type == 1539 else " up"
            print(f"{button_to_str[button]} {direction}")
            # the bitmask has been aligned with pygame button numbering,
            # so bit 1 = button 0, bit 2 = button 1, and so on
            # sets the proper bits for the bitmask representation of the controller state
            set_bit = (1 << button)
            new_value &= (~set_bit)  # resets bit flag for button
            if event_type == 1539:  # if the button is down, set bitmask
                new_value |= set_bit
        else:
            # ignore the event
            # print(event)
            continue
        # if the controller's state has changed, send an update to the Arduino
        if new_value != old_value:
            print(f"Sending value: f{new_value}, updated from: f{old_value}")
            peripheral.write_request(SERVICE_UUID, CHARACTERISTIC_UUID, new_value.to_bytes(2, "little"))
