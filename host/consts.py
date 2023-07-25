# UI command list
command_list = {
    0: 'BL_GET_CMD',
    1: 'BL_GET_VERSION',
    2: 'BL_GET_ID',
    3: 'BL_GET_PROTECT_LEVEL',
    4: 'BL_READ_MEM',
    5: 'BL_JUMP_TO_APP',
    6: 'BL_WRITE_MEM',
    7: 'BL_ERASE_MEM',
    8: 'BL_WRITE_PROTECT',
    9: 'BL_WRITE_UNPROTECT',
    10: 'BL_READ_PROTECT',
    11: 'BL_READ_UNPROTECT',
    12: 'BL_RELOAD_OPT_BYTES',
    13: 'Exit',
}

# bootloader command code list
BL_GET_CMD           = 0x00
BL_GET_VERSION       = 0x01
BL_GET_ID            = 0x02
BL_GET_PROTECT_LEVEL = 0x03
BL_READ_MEM          = 0x11
BL_JUMP_TO_APP       = 0x21
BL_WRITE_MEM         = 0x31
BL_ERASE_MEM         = 0x43
BL_WRITE_PROTECT     = 0x63
BL_WRITE_UNPROTECT   = 0x73
BL_READ_PROTECT      = 0x82
BL_READ_UNPROTECT    = 0x92
BL_RELOAD_OPT_BYTES  = 0xA1

# bootloader command buffer length
BL_GET_CMD_LEN           = 0
BL_GET_VERSION_LEN       = 0
BL_GET_ID_LEN            = 0
BL_GET_PROTECT_LEVEL_LEN = 0
BL_JUMP_TO_APP_LEN       = 0
BL_READ_MEM_LEN          = 5
BL_ERASE_MEM_LEN         = 2
BL_WRITE_MEM_MAX_LEN     = 255
BL_WRITE_UNPROTECT_LEN   = 0
BL_READ_PROTECT_LEN      = 1
BL_RELOAD_OPT_BYTES_LEN  = 0

# USART setting
USART_BAUDRATE = 38400

# CRC setting
CRC_INIT = 0xffffffff
CRC_POL = 0x04c11db7

# ACK/NACK
ACK = 0
NACK = 1

# maximum of bytes to send from binary file
MAX_BIN_LEN = 250
