/**
 * @file  bootloader.c
 * @brief functions about bootloader implementation
 */

#include <stdbool.h>
#include <string.h>
#include "common.h"
#include "crc.h"
#include "flash.h"
#include "usart.h"

#define BL_VERSION "1.0"
#define BL_ACK 0
#define BL_NACK 1
#define BL_CRC_SIZE 4
#define BL_BUFFER_SIZE 256

/* bootloader commands list
 * (command, code, reply length)
 */
#define BL_CMD_LIST               \
    _(get_cmd, 0x00, 10)          \
    _(get_version, 0x01, 3)       \
    _(get_id, 0x02, 4)            \
    _(get_protect_level, 0x03, 1) \
    _(read_mem, 0x11, 1)          \
    _(jump_to_app, 0x21, 0)       \
    _(write_mem, 0x31, 1)         \
    _(erase_mem, 0x43, 1)         \
    _(write_protect, 0x63, 1)     \
    _(write_unprotect, 0x73, 1)   \
    _(read_protect, 0x82, 1)      \
    _(reload_opt_bytes, 0xA1, 0)

/* unimplement command list */
#define BL_UNIMP_CMD_LIST _(read_unprotect, 0x92)

enum bl_cmd_code_list {
#define _(cmd, code, len) bl_##cmd##_cmd = code,
    BL_CMD_LIST
#undef _
};

/* bootloader commands reply length */
enum bl_cmd_reply_list {
#define _(cmd, code, len) bl_##cmd##_len = len,
    BL_CMD_LIST
#undef _
};

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
static void do_get_cmd(struct bl_command *command UNUSED)
{
    /* get all command code */
    uint8_t all_comand[] = {
#define _(cmd, code, len) bl_##cmd##_cmd,
        BL_CMD_LIST
#undef _
    };
    bl_send_data((uint8_t *) all_comand, bl_get_cmd_len);
}

/* handle BL_GET_VERSION
 * Get the protocol version
 */
static void do_get_version(struct bl_command *command UNUSED)
{
    /* get bootloader version */
    char *version = BL_VERSION;
    bl_send_data((uint8_t *) version, bl_get_version_len);
}

/* handle BL_GET_ID
 * Get the chip ID
 */
static void do_get_id(struct bl_command *command UNUSED)
{
    /* send rev_id */
    bl_send_data((uint8_t *) &DBGMCU, bl_get_id_len);
}

/* handle BL_GET_PROTECT_LEVEL
 * Get the protection level status
 */
static void do_get_protect_level(struct bl_command *command UNUSED)
{
    uint8_t protect_level = flash_get_protection_level();
    bl_send_data(&protect_level, bl_get_protect_level_len);
}

/* handle BL_READ_MEM
 * Read up to 256 bytes of memory starting from an address specified
 * by the application
 */
static void do_read_mem(struct bl_command *command UNUSED)
{
    /* decode buffer */
    uint32_t base_addr = *(uint32_t *) command->buffer;
    uint8_t len = command->buffer[4] + 1;
    uint8_t buffer[len];

    memcpy(buffer, (uint8_t *) base_addr, len);
    /* check read data correct */
    uint8_t res =
        !strncmp((const char *) buffer, (const char *) base_addr, len);

    bl_send_data(&res, bl_write_mem_len);
    /* send data */
    bl_send_data(buffer, len);
}

/* handle BL_JUMP_TO_APP
 * Jump to user application code located in the internal flash memory or
 * in the SRAM
 */
static void do_jump_to_app(struct bl_command *command UNUSED)
{
    /* reset the peripherals that bootloader uses */
    usart_reset();
    crc_reset();

    /* set MSP */
    uint32_t msp = *(volatile uint32_t *) APP_BASE;
    __asm volatile("MSR MSP, %0" ::"r"(msp));

    /* set vector table offset */
    VTOR = FLASH_BASE | VECT_TAB_OFF;

    /* get application reset handler address */
    uint32_t app_reset_addr = *(uint32_t *) (APP_BASE + 4);
    /* jump to application */
    void (*app_reset_handler)(void) = (void *) app_reset_addr;
    app_reset_handler();
}

/* handle BL_WRITE_MEM
 * Write up to 256 bytes to the RAM or flash memory starting from an
 * address specified by the application
 */
static void do_write_mem(struct bl_command *command UNUSED)
{
    /* decode buffer */
    uint32_t base_addr = *(uint32_t *) command->buffer;
    uint8_t bin_len = command->buffer[4];
    uint8_t *bin_data = command->buffer + 5;
    uint8_t res = 0;

    /* check flash operation is on */
    if (flash_is_on()) {
        /* send write failed to host */
        bl_send_data(&res, bl_erase_mem_len);
        return;
    }

    /* unlock sequence */
    flash_unlock_sequence();
    /* write flash */
    res = flash_write(base_addr, bin_data, bin_len);
    /* enable lock */
    flash_set_lock();

    bl_send_data(&res, bl_write_mem_len);
}

/* handle BL_ERASE_MEM
 * Erase from one to all the flash memory pages
 */
static void do_erase_mem(struct bl_command *command UNUSED)
{
    /* page number */
    uint8_t page = command->buffer[0];
    /* the number of page to erase */
    uint8_t page_num = command->buffer[1];
    uint8_t res = 0;

    /* check flash operation is on */
    if (flash_is_on()) {
        /* send erase failed to host */
        bl_send_data(&res, bl_erase_mem_len);
        return;
    }

    /* unlock sequence */
    flash_unlock_sequence();
    /* erase memory */
    res = (page == 0xFF) ? flash_mass_erase() : flash_erase(page, page_num);
    /* enable lock */
    flash_set_lock();

    bl_send_data(&res, bl_erase_mem_len);
}

/* handle BL_WRITE_PROTECT
 * Enable the write protection for some sectors
 */
static void do_write_protect(struct bl_command *command UNUSED)
{
    /* the number of page */
    uint8_t page_num = command->buffer[0];
    /* page number */
    uint8_t *page = command->buffer + 1;
    uint8_t res = 0;

    /* check flash operation is on */
    if (flash_is_on()) {
        /* send erase failed to host */
        bl_send_data(&res, bl_write_protect_len);
        return;
    }

    /* unlock sequence */
    flash_unlock_sequence();
    /* option unlock sequence */
    flash_opt_unlock_sequence();
    /* write protection */
    res = flash_write_protect(page, page_num);
    /* enable lock */
    flash_set_lock();

    bl_send_data(&res, bl_write_protect_len);
}

/* handle BL_WRITE_UNPROTECT
 * Disable the write protection for all flash memory sectors
 */
static void do_write_unprotect(struct bl_command *command UNUSED)
{
    uint8_t res = 0;

    /* check flash operation is on */
    if (flash_is_on()) {
        /* send erase failed to host */
        bl_send_data(&res, bl_write_unprotect_len);
        return;
    }

    /* unlock sequence */
    flash_unlock_sequence();
    /* option unlock sequence */
    flash_opt_unlock_sequence();
    /* write protection */
    res = flash_write_unprotect();
    /* enable lock */
    flash_set_lock();

    bl_send_data(&res, bl_write_unprotect_len);
}

/* handle BL_READ_PROTECT
 * Enable the read protection
 */
static void do_read_protect(struct bl_command *command UNUSED)
{
    /* read protection level */
    uint8_t level = command->buffer[0];
    uint8_t res = 0;

    /* check flash operation is on */
    if (flash_is_on()) {
        /* send read protection failed to host */
        bl_send_data(&res, bl_read_protect_len);
        return;
    }

    /* unlock sequence */
    flash_unlock_sequence();
    /* option unlock sequence */
    flash_opt_unlock_sequence();
    /* write protection */
    res = flash_read_protect(level);
    /* enable lock */
    flash_set_lock();

    bl_send_data(&res, bl_write_unprotect_len);
}

/* handle BL_READ_UNPROTECT
 * Disable the read protection
 */
static void do_read_unprotect(struct bl_command *command UNUSED) {}

/* handle BL_RELOAD_OPT_BYTES
 * Reload option bytes
 */
static void do_reload_opt_bytes(struct bl_command *command UNUSED)
{
    /* reload option bytes */
    flash_reload_opt_bytes();
}

#define CMD_HANDLER(cmd, len)                                    \
    static void bl_##cmd(struct bl_command *command)             \
    {                                                            \
        uint32_t crc = *(uint32_t *) command->crc;               \
        /* CRC verify failed */                                  \
        if (!bl_crc_verify(command, 2 + command->length, crc)) { \
            /* send nack to host */                              \
            bl_send_nack();                                      \
            return;                                              \
        }                                                        \
        /* send ack */                                           \
        bl_send_ack(len);                                        \
        /* execute command */                                    \
        do_##cmd(command);                                       \
    }

/* command handler */
#define _(cmd, code, len) CMD_HANDLER(cmd, len)
BL_CMD_LIST
#undef _

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
#define _(cmd, code, len)   \
    case bl_##cmd##_cmd:    \
        bl_##cmd(&command); \
        break;

        BL_CMD_LIST
#undef _
    default:
        break;
    }
}