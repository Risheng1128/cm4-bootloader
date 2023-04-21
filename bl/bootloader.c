/**
 * @file  bootloader.c
 * @brief functions about bootloader implementation
 */

#include <stdbool.h>
#include "crc.h"
#include "flash.h"
#include "usart.h"

#define BL_VERSION "1.0"
#define BL_ACK 0
#define BL_NACK 1
#define BL_CRC_SIZE 4
#define BL_BUFFER_SIZE 256

/* bootloader commands definition */
#define BL_GET_CMD 0x00
#define BL_GET_VERSION 0x01
#define BL_GET_ID 0x02
#define BL_GET_PROTECT_LEVEL 0x03
#define BL_READ_MEM 0x11
#define BL_JUMP_TO_APP 0x21
#define BL_WRITE_MEM 0x31
#define BL_ERASE_MEM 0x43
#define BL_ERASE_MEM_EXT 0x44
#define BL_SPECIAL 0x50
#define BL_SPECIAL_EXT 0x51
#define BL_WRITE_PROTECT 0x63
#define BL_WRITE_UNPROTECT 0x73
#define BL_READ_PROTECT 0x82
#define BL_READ_UNPROTECT 0x92
#define BL_GET_CHECKSUM 0xA1

/* bootloader commands reply length */
#define BL_GET_CMD_LEN 15
#define BL_GET_VERSION_LEN 3
#define BL_GET_ID_LEN 4
#define BL_GET_PROTECT_LEVEL_LEN 1
#define BL_JUMP_TO_APP_LEN 0
#define BL_ERASE_MEM_LEN 1

struct bl_command {
    uint8_t code;   /* command code */
    uint8_t length; /* buffer length */
    uint8_t buffer[BL_BUFFER_SIZE];
    uint8_t crc[BL_CRC_SIZE]; /* CRC data */
};

/* write data to host */
static void bl_send_data(uint8_t *buffer, uint8_t len)
{
    usart_send_data(buffer, len);
}

/* receive data from host */
static uint8_t bl_receive_data(void)
{
    return usart_receive_data();
}

/* send ack to host */
static void bl_send_ack(uint8_t len)
{
    uint8_t ack[2] = {BL_ACK, len};
    bl_send_data(ack, 2);
}

/* send nack to host */
static void bl_send_nack(void)
{
    uint8_t nack = BL_NACK;
    bl_send_data(&nack, 1);
}

static bool bl_crc_verify(struct bl_command *command,
                          uint16_t len,
                          uint32_t crc)
{
    uint32_t res;
    uint8_t *ptr = (uint8_t *) command;

    /* compute CRC value */
    for (uint16_t i = 0; i < len; i++) {
        res = crc_compute((uint32_t) ptr[i]);
    }

    /* reset CRC hardware */
    crc_reset();
    return res == crc;
}

/* handle BL_GET_CMD
 * Get the version and the allowed commands supported by the current
 * version of the protocol
 */
static void bl_get_cmd(struct bl_command *command)
{
    uint32_t crc = *(uint32_t *) command->crc;

    /* CRC verify failed */
    if (!bl_crc_verify(command, 2, crc)) {
        /* send nack to host */
        bl_send_nack();
        return;
    }

    /* send ack */
    bl_send_ack(BL_GET_CMD_LEN);

    /* get all command code */
    uint8_t all_comand[] = {
        BL_GET_CMD,      BL_GET_VERSION,    BL_GET_ID,
        BL_READ_MEM,     BL_JUMP_TO_APP,    BL_WRITE_MEM,
        BL_ERASE_MEM,    BL_ERASE_MEM_EXT,  BL_SPECIAL,
        BL_SPECIAL_EXT,  BL_WRITE_PROTECT,  BL_WRITE_UNPROTECT,
        BL_READ_PROTECT, BL_READ_UNPROTECT, BL_GET_CHECKSUM};
    bl_send_data((uint8_t *) all_comand, BL_GET_CMD_LEN);
}

/* handle BL_GET_VERSION
 * Get the protocol version
 */
static void bl_get_version(struct bl_command *command)
{
    uint32_t crc = *(uint32_t *) command->crc;

    /* CRC verify failed */
    if (!bl_crc_verify(command, 2, crc)) {
        /* send nack to host */
        bl_send_nack();
        return;
    }

    /* send ack */
    bl_send_ack(BL_GET_VERSION_LEN);

    /* get bootloader version */
    char *version = BL_VERSION;
    bl_send_data((uint8_t *) version, BL_GET_VERSION_LEN);
}

/* MCU device ID code */
#define DBGMCU *(volatile uint32_t *) 0xE0042000U

/* handle BL_GET_ID
 * Get the chip ID
 */
static void bl_get_id(struct bl_command *command)
{
    uint32_t crc = *(uint32_t *) command->crc;

    /* CRC verify failed */
    if (!bl_crc_verify(command, 2, crc)) {
        /* send nack to host */
        bl_send_nack();
        return;
    }

    /* send ack */
    bl_send_ack(BL_GET_ID_LEN);

    /* send rev_id */
    bl_send_data((uint8_t *) &DBGMCU, BL_GET_ID_LEN);
}

/* handle BL_GET_PROTECT_LEVEL
 * Get the protection level status
 */
static void bl_get_protect_level(struct bl_command *command)
{
    uint32_t crc = *(uint32_t *) command->crc;

    /* CRC verify failed */
    if (!bl_crc_verify(command, 2, crc)) {
        /* send nack to host */
        bl_send_nack();
        return;
    }

    /* send ack */
    bl_send_ack(BL_GET_PROTECT_LEVEL_LEN);

    uint8_t protect_level = (FLASH_OBR & 0x00000006) >> 1;
    bl_send_data(&protect_level, BL_GET_PROTECT_LEVEL_LEN);
}

/* handle BL_READ_MEM
 * Read up to 256 bytes of memory starting from an address specified
 * by the application
 */
static void bl_read_mem(struct bl_command *command) {}

/* handle BL_JUMP_TO_APP
 * Jump to user application code located in the internal flash memory or
 * in the SRAM
 */
static void bl_jump_to_app(struct bl_command *command)
{
    uint32_t crc = *(uint32_t *) command->crc;

    /* CRC verify failed */
    if (!bl_crc_verify(command, 2, crc)) {
        /* send nack to host */
        bl_send_nack();
        return;
    }

    /* send ack */
    bl_send_ack(BL_JUMP_TO_APP_LEN);

    /* reset the peripherals that bootloader uses */
    usart_reset();
    crc_reset();

    /* set MSP */
    uint32_t msp = *(volatile uint32_t *) FLASH_PAGE_8_BASE;
    __asm volatile("MSR MSP, %0" ::"r"(msp));

    /* get application reset handler address */
    uint32_t app_reset_addr = *(uint32_t *) (FLASH_PAGE_8_BASE + 4);

    /* set vector table offset */
    VTOR = FLASH_BASE | VECT_TAB_OFF;

    /* jump to application */
    void (*app_reset_handler)(void) = (void *) app_reset_addr;
    app_reset_handler();
}

/* handle BL_WRITE_MEM
 * Write up to 256 bytes to the RAM or flash memory starting from an
 * address specified by the application
 */
static void bl_write_mem(struct bl_command *command) {}

/* handle BL_ERASE_MEM
 * Erase from one to all the flash memory pages
 */
static void bl_erase_mem(struct bl_command *command)
{
    uint32_t crc = *(uint32_t *) command->crc;

    /* CRC verify failed */
    if (!bl_crc_verify(command, 2 + command->length, crc)) {
        /* send nack to host */
        bl_send_nack();
        return;
    }

    /* send ack */
    bl_send_ack(BL_ERASE_MEM_LEN);

    // /* page number */
    uint8_t page = command->buffer[0];
    /* the number of page to erase */
    uint8_t page_num = command->buffer[1];

    /* check lock bit */
    if (FLASH_CR & 0x00000080) {
        flash_unlock_sequence();
    }

    uint8_t res =
        (page == 0xFF) ? flash_mass_erase() : flash_erase(page, page_num);
    /* enable lock */
    SET_LOCK();

    bl_send_data(&res, BL_ERASE_MEM_LEN);
}

/* handle BL_ERASE_MEM_EXT
 * Erase from one to all the flash memory pages using two-byte addressing mode
 */
static void bl_erase_mem_ext(struct bl_command *command) {}

/* handle BL_SPECIAL
 * Generic command that allows to add new features depending on the product
 * constraints, without adding a new command for every feature
 */
static void bl_special(struct bl_command *command) {}

/* handle BL_SPECIAL_EXT
 * Generic command that allows the user to send more data compared to
 * the special command
 */
static void bl_special_ext(struct bl_command *command) {}

/* handle BL_WRITE_PROTECT
 * Enable the write protection for some sectors
 */
static void bl_write_protect(struct bl_command *command) {}

/* handle BL_WRITE_UNPROTECT
 * Disable the write protection for all flash memory sectors
 */
static void bl_write_unprotect(struct bl_command *command) {}

/* handle BL_READ_PROTECT
 * Enable the read protection
 */
static void bl_read_protect(struct bl_command *command) {}

/* handle BL_READ_UNPROTECT
 * Disable the read protection
 */
static void bl_read_unprotect(struct bl_command *command) {}

/* handle BL_GET_CHECKSUM
 * Compute a CRC value on a given memory area with a size multiple of 4 bytes
 */
static void bl_get_checksum(struct bl_command *command) {}

/* read command from USART */
void bl_read_command(void)
{
    struct bl_command command;
    /* read command code */
    command.code = bl_receive_data();
    /* read command buffer length */
    command.length = bl_receive_data();

    /* read buffer data */
    for (uint8_t i = 0; i < command.length; i++) {
        command.buffer[i] = bl_receive_data();
    }
    /* read CRC data */
    for (uint8_t i = 0; i < BL_CRC_SIZE; i++) {
        command.crc[i] = bl_receive_data();
    }

    /* handle bootloader commands */
    switch (command.code) {
    case BL_GET_CMD:
        bl_get_cmd(&command);
        break;
    case BL_GET_VERSION:
        bl_get_version(&command);
        break;
    case BL_GET_ID:
        bl_get_id(&command);
        break;
    case BL_GET_PROTECT_LEVEL:
        bl_get_protect_level(&command);
        break;
    case BL_READ_MEM:
        bl_read_mem(&command);
        break;
    case BL_JUMP_TO_APP:
        bl_jump_to_app(&command);
        break;
    case BL_WRITE_MEM:
        bl_write_mem(&command);
        break;
    case BL_ERASE_MEM:
        bl_erase_mem(&command);
        break;
    case BL_ERASE_MEM_EXT:
        bl_erase_mem_ext(&command);
        break;
    case BL_SPECIAL:
        bl_special(&command);
        break;
    case BL_SPECIAL_EXT:
        bl_special_ext(&command);
        break;
    case BL_WRITE_PROTECT:
        bl_write_protect(&command);
        break;
    case BL_WRITE_UNPROTECT:
        bl_write_unprotect(&command);
        break;
    case BL_READ_PROTECT:
        bl_read_protect(&command);
        break;
    case BL_READ_UNPROTECT:
        bl_read_unprotect(&command);
        break;
    case BL_GET_CHECKSUM:
        bl_get_checksum(&command);
        break;
    default:
        break;
    }
}