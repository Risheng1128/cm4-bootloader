# bootloader command
BL_GET_CMD           = 0x00
BL_GET_VERSION       = 0x01
BL_GET_ID            = 0x02
BL_GET_PROTECT_LEVEL = 0x03
BL_READ_MEM          = 0x11
BL_JUMP_TO_APP       = 0x21
BL_WRITE_MEM         = 0x31
BL_ERASE_MEM         = 0x43
BL_ERASE_MEM_EXT     = 0x44
BL_SPECIAL           = 0x50
BL_SPECIAL_EXT       = 0x51
BL_WRITE_PROTECT     = 0x63
BL_WRITE_UNPROTECT   = 0x73
BL_READ_PROTECT      = 0x82
BL_READ_UNPROTECT    = 0x92
BL_GET_CHECKSUM      = 0xA1

# bootloader command buffer length
BL_GET_CMD_LEN           = 0
BL_GET_VERSION_LEN       = 0
BL_GET_ID_LEN            = 0
BL_GET_PROTECT_LEVEL_LEN = 0
BL_JUMP_TO_APP_LEN       = 0
BL_ERASE_MEM_LEN         = 2
BL_WRITE_MEM_MAX_LEN     = 255

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
