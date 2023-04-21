import os
import utility
import consts as const

class command_package:
    def __init__(self, code, buffer_len):
        self.cmd = bytearray()
        # add command code
        self.append(code)
        # add command buffer length
        self.append(buffer_len)

    def append(self, data):
        self.cmd.append(data)

    # append 4 bytes data
    def append_int(self, data):
        for i in data.to_bytes(4, 'little'):
            self.cmd.append(i)

    def append_bytes_array(self, array):
        for i in array:
            self.cmd.append(i)

    # append CRC value
    def append_crc(self):
        crc = utility.compute_crc(self.cmd)
        self.append_int(crc)

    # send command
    def send_command(self, port):
        utility.serial_port_write(port, self.cmd)

'''
Get the version and the allowed commands supported by the current
version of the protocol
'''
def do_get_cmd(port):
    buffer = command_package(const.BL_GET_CMD,
                             const.BL_GET_CMD_LEN)
    buffer.append_crc()
    buffer.send_command(port)

    # read ACK/NACK
    if utility.serial_read_to_int(port, 1) == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_read_to_int(port, 1)
    reply = utility.serial_port_read(port, reply_len)
    print(' Support Command Code: ', [hex(x) for x in reply])

'''
Get the protocol version
'''
def do_get_version(port):
    buffer = command_package(const.BL_GET_VERSION,
                             const.BL_GET_VERSION_LEN)
    buffer.append_crc()
    buffer.send_command(port)

    # read ACK/NACK
    if utility.serial_read_to_int(port, 1) == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_read_to_int(port, 1)
    reply = utility.serial_port_read(port, reply_len)
    print(' Bootloader Version: ', reply.decode('ascii'))

'''
Get the chip ID
'''
def do_get_id(port):
    buffer = command_package(const.BL_GET_ID,
                             const.BL_GET_ID_LEN)
    buffer.append_crc()
    buffer.send_command(port)
    print(' Read chip id ...')

    # read ACK/NACK
    if utility.serial_read_to_int(port, 1) == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_read_to_int(port, 1)
    reply = utility.serial_read_to_int(port, reply_len)
    print(' REV ID: ', hex((reply & 0xffff0000) >> 16))
    print(' DEV ID: ', hex(reply & 0x00000fff))

'''
Get the protection level status
'''
def do_get_protect_level(port):
    buffer = command_package(const.BL_GET_PROTECT_LEVEL,
                             const.BL_GET_PROTECT_LEVEL_LEN)
    buffer.append_crc()
    buffer.send_command(port)
    print(' Read protection level status ...')

    # read ACK/NACK
    if utility.serial_read_to_int(port, 1) == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_read_to_int(port, 1)
    reply = utility.serial_read_to_int(port, reply_len)
    if reply == 0:
        print(' Protection Level 0 is enabled')
    elif reply == 1:
        print(' Protection Level 1 is enabled')
    elif reply == 3:
        print(' Protection Level 2 is enabled')

'''
Read up to 256 bytes of memory starting from an address specified
by the application
'''
def do_read_mem(port):
    pass

'''
Jump to user application code located in the internal flash memory or
in the SRAM
'''
def do_jump_to_app(port):
    buffer = command_package(const.BL_JUMP_TO_APP,
                             const.BL_JUMP_TO_APP_LEN)
    buffer.append_crc()
    buffer.send_command(port)

    # read ACK/NACK
    if utility.serial_read_to_int(port, 1) == const.NACK:
        print(' Read NACK')
        return

    print(' Jump to application ...')
    raise SystemExit

'''
Write up to 256 bytes to the RAM or flash memory starting from an
address specified by the application
'''
def send_write_mem_cmd(port, buf_len, addr, bin_len, bin_data):
    buffer = command_package(const.BL_WRITE_MEM,
                             buf_len)
    # add base address
    buffer.append_int(addr)
    # add bin_len
    buffer.append(bin_len)
    # add binary data
    buffer.append_bytes_array(bin_data)
    buffer.append_crc()
    buffer.send_command(port)

    # read ACK/NACK
    if utility.serial_read_to_int(port, 1) == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_read_to_int(port, 1)
    if utility.serial_read_to_int(port, reply_len):
        print(' Write memory success')
    else:
        print(' Write memory failed')

'''
Send a binary file to the assigned address
'''
def do_write_mem(port):
    file_path = input(' Type the file path: ')
    addr = int(input(' Type the base address: '), 16)

    # open the binary file
    print(' open file: ', file_path)
    with open(file_path, 'rb') as file:
        bin_data = file.read()

    bin_len = bin_data.__len__()
    quot = int(bin_len / const.MAX_BIN_LEN)
    remain = bin_len % const.MAX_BIN_LEN

    index = 0
    print(' Write binary file ...')
    # binary file exceed 250 bytes
    for i in range(quot):
        print(' round', i + 1, ': ')
        # send write memory command
        send_write_mem_cmd(port, const.BL_WRITE_MEM_MAX_LEN, addr,
                           const.MAX_BIN_LEN,
                           bin_data[index : index + const.MAX_BIN_LEN])

        addr += const.MAX_BIN_LEN
        index += const.MAX_BIN_LEN

    print(' final round : ')
    send_write_mem_cmd(port, remain + 5, addr, remain,
                       bin_data[index : index + remain])

'''
Erase from one to all the flash memory pages
'''
def do_erase_mem(port):
    page = int(input(' Type the page number (0 ~ 255): '))
    page_num = int(input(' Type the number of page (0 ~ 255): '))
    # check the page number and the number of page
    if (page & 0xffffff00) or (page_num & 0xffffff00):
        print(' Incorrect page number or page length')
        return
    if page + page_num > 255:
        print(' Exceed page number limit')
        return

    buffer = command_package(const.BL_ERASE_MEM,
                             const.BL_ERASE_MEM_LEN)
    # add buffer data
    buffer.append(page)
    buffer.append(page_num)
    buffer.append_crc()
    buffer.send_command(port)
    print(' Erase memory ...')

    # read ACK/NACK
    if utility.serial_read_to_int(port, 1) == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_read_to_int(port, 1)
    if utility.serial_read_to_int(port, reply_len):
        print(' Erase memory success')
    else:
        print(' Erase memory failed')

'''
Erase from one to all the flash memory pages using two-byte addressing mode
'''
def do_erase_mem_ext(port):
    pass

'''
Generic command that allows to add new features depending on the product
constraints, without adding a new command for every feature
'''
def do_special(port):
    pass

'''
Generic command that allows the user to send more data compared to
the special command
'''
def do_special_ext(port):
    pass

'''
Enable the write protection for some sectors
'''
def do_write_protect(port):
    pass

'''
Disable the write protection for all flash memory sectors
'''
def do_write_unprotect(port):
    pass

'''
Enable the read protection
'''
def do_read_protect(port):
    pass

'''
Disable the read protection
'''
def do_read_unprotect(port):
    pass

'''
Compute a CRC value on a given memory area with a size multiple of 4 bytes
'''
def do_get_checksum(port):
    pass

def decode_command_code(port, command_code):
    match (command_code):
        case (0):  # Menu exit
            print(' Exiting ...')
            raise SystemExit
        case (1):  # BL_GET_CMD
            print(' Command --> BL_GET_CMD')
            do_get_cmd(port)
        case (2):  # BL_GET_VERSION
            print(' Command --> BL_GET_VERSION')
            do_get_version(port)
        case (3):  # BL_GET_ID
            print(' Command --> BL_GET_ID')
            do_get_id(port)
        case (4):  # BL_GET_PROTECT_LEVEL
            print(' Command --> BL_GET_PROTECT_LEVEL')
            do_get_protect_level(port)
        case (5):  # BL_READ_MEM
            print(' Command --> BL_READ_MEM')
            do_read_mem(port)
        case (6):  # BL_JUMP_TO_APP
            print(' Command --> BL_JUMP_TO_APP')
            do_jump_to_app(port)
        case (7):  # BL_WRITE_MEM
            print(' Command --> BL_WRITE_MEM')
            do_write_mem(port)
        case (8):  # BL_ERASE_MEM
            print(' Command --> BL_ERASE_MEM')
            do_erase_mem(port)
        case (9):  # BL_ERASE_MEM_EXT
            print(' Command --> BL_ERASE_MEM_EXT')
            do_erase_mem_ext(port)
        case (10):  # BL_SPECIAL
            print(' Command --> BL_SPECIAL')
            do_special(port)
        case (11): # BL_SPECIAL_EXT
            print(' Command --> BL_SPECIAL_EXT')
            do_special_ext(port)
        case (12): # BL_WRITE_PROTECT
            print(' Command --> BL_WRITE_PROTECT')
            do_write_protect(port)
        case (13): # BL_WRITE_UNPROTECT
            print(' Command --> BL_WRITE_UNPROTECT')
            do_write_unprotect(port)
        case (14): # BL_READ_PROTECT
            print(' Command --> BL_READ_PROTECT')
            do_read_protect(port)
        case (15): # BL_READ_UNPROTECT
            print(' Command --> BL_READ_UNPROTECT')
            do_read_unprotect(port)
        case (16): # BL_GET_CHECKSUM
            print(' Command --> BL_GET_CHECKSUM')
            do_get_checksum(port)
        case (17):
            # display all valid serial ports
            print(' show valid ports... ')
            utility.serial_port_list()
        case (18):
            os.system('clear')
        case (19):
            # display command menu
            utility.print_menu()
        case _:
            pass