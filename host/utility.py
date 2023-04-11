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
    print(" | BL_READ_MEM                       --> 4  |")
    print(" | BL_JUMP_TO_APP                    --> 5  |")
    print(" | BL_WRITE_MEM                      --> 6  |")
    print(" | BL_ERASE_MEM                      --> 7  |")
    print(" | BL_ERASE_MEM_EXT                  --> 8  |")
    print(" | BL_SPECIAL                        --> 9  |")
    print(" | BL_SPECIAL_EXT                    --> 10 |")
    print(" | BL_WRITE_PROTECT                  --> 11 |")
    print(" | BL_WRITE_UNPROTECT                --> 12 |")
    print(" | BL_READ_PROTECT                   --> 13 |")
    print(" | BL_READ_UNPROTECT                 --> 14 |")
    print(" | BL_GET_CHECKSUM                   --> 15 |")
    print(" | SHOW VALID PORT                   --> 16 |")
    print(" | CLEAR SCREEN                      --> 17 |")
    print(" | SHOW COMMAND LIST                 --> 18 |")
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
