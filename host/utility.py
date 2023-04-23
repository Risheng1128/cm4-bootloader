import serial
import curses
import glob
import os
import consts as const
import command

# display user menu
def print_menu(screen, curr):
    screen.addstr(" +------------------------------------------+\n")
    screen.addstr(" |                   Menu                   |\n")
    screen.addstr(" |              CM4 BootLoader              |\n")
    screen.addstr(" +------------------------------------------+\n")

    for key, cmd in const.command_list.items():
        if key == curr:
            screen.addstr(" | > " + cmd + ' '*(39 - len(cmd)) + "|\n",
                          curses.A_REVERSE)
        else:
            screen.addstr(" |   " + cmd + ' '*(39 - len(cmd)) + "|\n")

    screen.addstr(" +------------------------------------------+\n")

# select the input of user
def select_key(port, screen, key, curr):
    if key == curses.KEY_UP:
        curr = (curr - 1) % len(const.command_list)
    elif key == curses.KEY_DOWN:
        curr = (curr + 1) % len(const.command_list)
    elif key == ord('\n'): # enter
        screen_deinit(screen)
        command.decode_command_code(port, screen, const.command_list[curr])
        os.system('bash -c \'read -p " Press any to continue"\'')
        os.system('clear')
    return curr

# UI initialize
def screen_init():
    screen = curses.initscr()
    screen.keypad(True)
    curses.cbreak()
    curses.noecho()
    return screen

# UI release
def screen_deinit(screen):
    screen.keypad(False)
    curses.nocbreak()
    curses.echo()
    curses.endwin()

# display all valid serial ports
def serial_port_list():
    # linux environment
    ports = glob.glob('/dev/tty[A-Za-z]*')
    for port in ports:
        print(' ', port)

def serial_port_setting(name):
    try:
        port = serial.Serial(name, const.USART_BAUDRATE)
    except:
        raise AssertionError('invalid port')

    if not port.is_open:
        raise AssertionError('port open failed')

    print('port open success')
    return port

def serial_port_write(port, buffer):
    port.write(buffer)

def serial_port_read(port, len):
    return port.read(len)

# read bytes from serial port and convert to integer
def serial_read_to_int(port, len):
    data = serial_port_read(port, len)
    return int.from_bytes(data, 'little')

# compute CRC value
def compute_crc(buffer):
    crc = const.CRC_INIT
    for byte in buffer:
        crc = crc ^ byte
        for i in range(32):
            if(crc & 0x80000000):
                crc = (crc << 1) ^ const.CRC_POL
            else:
                crc <<= 1
    return crc & 0xffffffff
