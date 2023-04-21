import os
import utility
import consts as const

'''
Get the version and the allowed commands supported by the current
version of the protocol
'''
def do_get_cmd(port):
    buffer = bytearray()
    # add command code
    buffer.append(const.BL_GET_CMD)
    # add command buffer length
    buffer.append(const.BL_GET_CMD_LEN)
    crc = utility.compute_crc(buffer)
    for i in crc.to_bytes(4, 'little'):
        buffer.append(i)

    # send command
    utility.serial_port_write(port, buffer)

    # read ACK/NACK
    ack_or_nack = utility.serial_port_read(port, 1)
    ack_or_nack = int.from_bytes(ack_or_nack, 'little')
    if ack_or_nack == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_port_read(port, 1)
    reply_len = int.from_bytes(reply_len, 'little')
    reply = utility.serial_port_read(port, reply_len)
    reply = [hex(x) for x in reply]
    print(' Support Command Code: ', reply)

'''
Get the protocol version
'''
def do_get_version(port):
    buffer = bytearray()
    # add command code
    buffer.append(const.BL_GET_VERSION)
    # add command buffer length
    buffer.append(const.BL_GET_VERSION_LEN)
    crc = utility.compute_crc(buffer)
    for i in crc.to_bytes(4, 'little'):
        buffer.append(i)

    # send command
    utility.serial_port_write(port, buffer)

    # read ACK/NACK
    ack_or_nack = utility.serial_port_read(port, 1)
    ack_or_nack = int.from_bytes(ack_or_nack, 'little')
    if ack_or_nack == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_port_read(port, 1)
    reply_len = int.from_bytes(reply_len, 'little')
    reply = utility.serial_port_read(port, reply_len)
    print(' Bootloader Version: ', reply.decode('ascii'))

'''
Get the chip ID
'''
def do_get_id(port):
    buffer = bytearray()
    # add command code
    buffer.append(const.BL_GET_ID)
    # add command buffer length
    buffer.append(const.BL_GET_ID_LEN)
    crc = utility.compute_crc(buffer)
    for i in crc.to_bytes(4, 'little'):
        buffer.append(i)

    # send command
    utility.serial_port_write(port, buffer)

    # read ACK/NACK
    ack_or_nack = utility.serial_port_read(port, 1)
    ack_or_nack = int.from_bytes(ack_or_nack, 'little')
    if ack_or_nack == const.NACK:
        print(' Read NACK')
        return

    print(' Read chip id ...')
    reply_len = utility.serial_port_read(port, 1)
    reply_len = int.from_bytes(reply_len, 'little')
    reply = utility.serial_port_read(port, reply_len)
    id = int.from_bytes(reply, 'little')
    print(' REV ID: ', hex((id & 0xffff0000) >> 16))
    print(' DEV ID: ', hex(id & 0x00000fff))

'''
Get the protection level status
'''
def do_get_protect_level(port):
    buffer = bytearray()
    # add command code
    buffer.append(const.BL_GET_PROTECT_LEVEL)
    # add command buffer length
    buffer.append(const.BL_GET_PROTECT_LEVEL_LEN)
    crc = utility.compute_crc(buffer)
    for i in crc.to_bytes(4, 'little'):
        buffer.append(i)

    # send command
    utility.serial_port_write(port, buffer)

    # read ACK/NACK
    ack_or_nack = utility.serial_port_read(port, 1)
    ack_or_nack = int.from_bytes(ack_or_nack, 'little')
    if ack_or_nack == const.NACK:
        print(' Read NACK')
        return

    print(' Read protection level status ...')
    reply_len = utility.serial_port_read(port, 1)
    reply_len = int.from_bytes(reply_len, 'little')
    reply = utility.serial_port_read(port, reply_len)
    id = int.from_bytes(reply, 'little')
    if id == 0:
        print(' Protection Level 0 is enabled')
    elif id == 1:
        print(' Protection Level 1 is enabled')
    elif id == 3:
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
    buffer = bytearray()
    # add command code
    buffer.append(const.BL_JUMP_TO_APP)
    # add command buffer length
    buffer.append(const.BL_JUMP_TO_APP_LEN)
    crc = utility.compute_crc(buffer)
    for i in crc.to_bytes(4, 'little'):
        buffer.append(i)

    # send command
    utility.serial_port_write(port, buffer)

    # read ACK/NACK
    ack_or_nack = utility.serial_port_read(port, 1)
    ack_or_nack = int.from_bytes(ack_or_nack, 'little')
    if ack_or_nack == const.NACK:
        print(' Read NACK')
        return

    print(' Jump to application...')
    raise SystemExit

'''
Write up to 256 bytes to the RAM or flash memory starting from an
address specified by the application
'''
def send_write_mem_cmd(port, buf_len, addr, bin_len, bin_data):
    buffer = bytearray()
    # add command code
    buffer.append(const.BL_WRITE_MEM)
    # add command buffer length
    buffer.append(buf_len)
    # add base address
    for i in addr.to_bytes(4, 'little'):
        buffer.append(i)
    # add bin_len
    buffer.append(bin_len)
    # add binary data
    for i in bin_data:
        buffer.append(i)
    crc = utility.compute_crc(buffer)
    for i in crc.to_bytes(4, 'little'):
        buffer.append(i)

    # send command
    utility.serial_port_write(port, buffer)

    # read ACK/NACK
    ack_or_nack = utility.serial_port_read(port, 1)
    ack_or_nack = int.from_bytes(ack_or_nack, 'little')
    # check ACK/NACK
    if ack_or_nack == const.NACK:
        print(' Read NACK')
        return

    reply_len = utility.serial_port_read(port, 1)
    reply_len = int.from_bytes(reply_len, 'little')
    reply = utility.serial_port_read(port, reply_len)
    reply = int.from_bytes(reply, 'little')
    if reply:
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

    print(' final round: ')
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

    buffer = bytearray()
    # add command code
    buffer.append(const.BL_ERASE_MEM)
    # add command buffer length
    buffer.append(const.BL_ERASE_MEM_LEN)
    # add buffer data
    buffer.append(page)
    buffer.append(page_num)
    crc = utility.compute_crc(buffer)
    for i in crc.to_bytes(4, 'little'):
        buffer.append(i)

    # send command
    utility.serial_port_write(port, buffer)

    # read ACK/NACK
    ack_or_nack = utility.serial_port_read(port, 1)
    ack_or_nack = int.from_bytes(ack_or_nack, 'little')
    if ack_or_nack == const.NACK:
        print(' Read NACK')
        return

    print(' Erase memory ...')
    reply_len = utility.serial_port_read(port, 1)
    reply_len = int.from_bytes(reply_len, 'little')
    reply = utility.serial_port_read(port, reply_len)
    reply = int.from_bytes(reply, 'little')
    if reply:
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