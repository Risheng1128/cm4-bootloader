import serial
import glob
import consts as const

# display user menu
def print_menu():
    print(" +------------------------------------------+")
    print(" |                   Menu                   |")
    print(" |              CM4 BootLoader              |")
    print(" +------------------------------------------+")
    print(" | MENU_EXIT                         --> 0  |")
    print(" | BL_GET_CMD                        --> 1  |")
    print(" | BL_GET_VERSION                    --> 2  |")
    print(" | BL_GET_ID                         --> 3  |")
    print(" | BL_GET_PROTECT_LEVEL              --> 4  |")
    print(" | BL_READ_MEM                       --> 5  |")
    print(" | BL_JUMP_TO_APP                    --> 6  |")
    print(" | BL_WRITE_MEM                      --> 7  |")
    print(" | BL_ERASE_MEM                      --> 8  |")
    print(" | BL_ERASE_MEM_EXT                  --> 9  |")
    print(" | BL_SPECIAL                        --> 10 |")
    print(" | BL_SPECIAL_EXT                    --> 11 |")
    print(" | BL_WRITE_PROTECT                  --> 12 |")
    print(" | BL_WRITE_UNPROTECT                --> 13 |")
    print(" | BL_READ_PROTECT                   --> 14 |")
    print(" | BL_READ_UNPROTECT                 --> 15 |")
    print(" | BL_GET_CHECKSUM                   --> 16 |")
    print(" | SHOW VALID PORT                   --> 17 |")
    print(" | CLEAR SCREEN                      --> 18 |")
    print(" | SHOW COMMAND LIST                 --> 19 |")
    print(" +------------------------------------------+")

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
