import ctypes
import simplepyble
from simplepyble import Peripheral
from sdl2 import SDL_Event, SDL_PollEvent, SDL_JoystickOpen, SDL_Init, SDL_INIT_JOYSTICK, SDL_INIT_EVENTS, \
    SDL_INIT_VIDEO
from time import sleep

SERVICE_UUID = '1ae49b08-b750-4ef7-afd8-5395763c0da6'
CHARACTERISTIC_UUID = '19b10011-e8f2-537e-4f6c-d104768a1214'


def get_adapter():
    adapters = simplepyble.Adapter.get_adapters()

    if len(adapters) == 0:
        print("No adapters found")
        return

    if len(adapters) == 1:
        adapter = adapters[0]
    else:
        # Query the user to pick an adapter
        print("Please select an adapter:")
        for i, adapter in enumerate(adapters):
            print(f"{i}: {adapter.identifier()} [{adapter.address()}]")

        choice = int(input("Enter choice: "))
        adapter = adapters[choice]

    adapter.set_callback_on_scan_start(lambda: print("Scanning for bluetooth devices."))
    adapter.set_callback_on_scan_stop(lambda: print("Scan complete."))
    adapter.set_callback_on_scan_found(
        lambda peripheral: print(f"Found {peripheral.identifier()} [{peripheral.address()}]"))
    return adapter


def get_peripheral(adapter, identifier) -> Peripheral:
    peripheral = None
    while True:
        # Scan for 5 seconds
        adapter.scan_for(5000)
        peripherals = adapter.scan_get_results()
        # # Query the user to pick a peripheral
        # print("Please select a peripheral:")
        # for i, peripheral in enumerate(peripherals):
        #     print(f"{i}: {peripheral.identifier()} [{peripheral.address()}]")
        #
        # choice = int(input("Enter choice or -1 to scan again: "))
        # if choice == -1:
        #     continue
        # peripheral = peripherals[choice]
        for p in peripherals:
            if p.identifier() == identifier:
                peripheral = p
                break
        if peripheral is not None:
            break

    print(f"Connecting to: {peripheral.identifier()} [{peripheral.address()}]")
    peripheral.connect()
    print("Connected to Arduino")
    return peripheral


def dpad_input_to_str(old_val: int, new_val: int) -> str:
    change = old_val ^ new_val
    ret = ""
    ret += "top" if change == 1 else "right" if change == 2 else "bottom" if change == 4 else "left"
    return ret + " down" if change & new_val == change else " up"


button_to_str = ["A button", "B button", "X button", "Y button", "Left bumper", "Right bumper", "Select", "Start",
                 "Left joystick", "Right joystick", "Xbox button", "Button 12"]

team_number = input("Enter your team number: ")
adapter = get_adapter()
peripheral = get_peripheral(adapter, f"BotathonTeam{team_number}")
print("Initializing SDL2")
SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS | SDL_INIT_VIDEO)
print("SDL2 initialized")
while True:
    print("Waiting for controller input...")
    event = SDL_Event()
    while SDL_PollEvent(ctypes.byref(event)):
        event_type = event.type
        old_value = int.from_bytes(peripheral.read(SERVICE_UUID, CHARACTERISTIC_UUID), "little")
        new_value = old_value
        if event_type == 1541:
            SDL_JoystickOpen(0)
        elif event_type == 1538:  # Input is D-Pad
            print(f"D-pad {dpad_input_to_str(event.jbutton.state)}")
            set_bits = event.jbutton.state << 12
            new_value = (new_value & 0b0000111111111111  # value & 4095 (b10)... resets the d-pad bits
                         ) | set_bits  # sets d-pad bits
            # print(set_bits)
        elif event_type == 1539 or event_type == 1540:  # Input is button up/down
            button = event.jbutton.button
            print(f"{button_to_str[button]} {" down" if event_type == 1539 else " up"}")
            # the bitmask has been aligned with pygame button numbering,
            # so bit 1 = button 0, bit 2 = button 1, and so on
            set_bit = (1 << button)
            new_value &= (~set_bit)  # resets bit flag for button
            if event_type == 1539:  # if the button is down, set bitmask
                new_value |= set_bit
        else:
            # print(event)
            continue
        if new_value != old_value:
            print(f"Sending value: f{new_value}, updated from: f{old_value}")
            peripheral.write_request(SERVICE_UUID, CHARACTERISTIC_UUID, new_value.to_bytes(2, "little"))
