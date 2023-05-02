/**
 * @file  flash.c
 * @brief functions about flash implementation
 */

#include "flash.h"

#define FLASH_UNLOCK_KEY1 0x45670123U
#define FLASH_UNLOCK_KEY2 0xCDEF89ABU
#define FLASH_OPT_BYTES_BASE 0x1FFFF800U

/* Option byte organization
 * address    | [31:24] | [23:16] | [15:8]  | [7:0]
 * 0x1FFFF800 |  nUser  |  User   |  nRDP   |  RDP
 * 0x1FFFF804 |  nData1 |  Data   |  nData0 |  Data0
 * 0x1FFFF808 |  nWRP1  |  WRP1   |  nWRP0  |  WRP0
 * 0x1FFFF80C |  nWRP3  |  WRP3   |  nWRP2  |  WRP2
 */
struct opt_bytes {
    uint16_t rdp;
    uint16_t user;
    uint16_t data[2];
    uint16_t wrp[4];
};

/* wait flash operation complete */
#define WAIT_BSY()                      \
    do {                                \
        while (FLASH_SR & (0x00000001)) \
            ;                           \
    } while (0)

/* perform unlock sequence */
void flash_unlock_sequence(void)
{
    FLASH_KEYR = FLASH_UNLOCK_KEY1;
    FLASH_KEYR = FLASH_UNLOCK_KEY2;
}

/* perform option unlock sequence */
void flash_opt_unlock_sequence(void)
{
    FLASH_OPTKEYR = FLASH_UNLOCK_KEY1;
    FLASH_OPTKEYR = FLASH_UNLOCK_KEY2;
}

/* perform write operation */
bool flash_write(uint32_t base_addr, uint8_t *buffer, uint8_t len)
{
    /* enable flash programming */
    FLASH_CR |= 1 << 0;

    uint16_t *addr = (uint16_t *) base_addr;
    for (uint8_t i = 0; i < len; i += 2) {
        /* write to flash (half-word) */
        *addr++ = *(uint16_t *) (buffer + i);

        /* wait flash operation complete */
        WAIT_BSY();

        /* check EOP */
        if (!(FLASH_SR & 0x00000020)) {
            /* disable page erase */
            FLASH_CR &= ~(1 << 0);
            return false;
        }
        /* reset EOP */
        FLASH_SR |= 1 << 5;
    }

    /* disable flash programming */
    FLASH_CR &= ~(1 << 0);
    return true;
}

/* perform erase operation */
bool flash_erase(uint8_t page, uint8_t page_num)
{
    /* enable page erase */
    FLASH_CR |= 1 << 1;

    for (uint8_t i = 0; i < page_num; i++) {
        /* set page number to erase */
        uint32_t addr = FLASH_BASE + (page + i) * FLASH_PAGE_SIZE;
        FLASH_AR = addr;

        /* start erase operation */
        FLASH_CR |= 1 << 6;

        /* wait flash operation complete */
        WAIT_BSY();

        /* check EOP */
        if (!(FLASH_SR & 0x00000020)) {
            /* disable page erase */
            FLASH_CR &= ~(1 << 1);
            return false;
        }
        /* reset EOP */
        FLASH_SR |= 1 << 5;
    }

    /* disable page erase */
    FLASH_CR &= ~(1 << 1);
    return true;
}

/* perform mass erase operation */
bool flash_mass_erase(void)
{
    /* enable mass erase page */
    FLASH_CR |= 1 << 2;

    /* start erase operation */
    FLASH_CR |= 1 << 6;

    /* wait flash operation complete */
    WAIT_BSY();

    /* check EOP */
    if (!(FLASH_SR & 0x00000020)) {
        /* disable mass erase page */
        FLASH_CR &= ~(1 << 2);
        return false;
    }
    /* reset EOP */
    FLASH_SR |= 1 << 5;

    /* disable mass erase page */
    FLASH_CR &= ~(1 << 2);
    return true;
}

/* perform option bytes erase operation */
static bool flash_opt_erase(void)
{
    /* check flash operation is on */
    if (flash_is_on()) {
        return false;
    }

    /* enable option byte erase */
    FLASH_CR |= 1 << 5;

    /* start erase operation */
    FLASH_CR |= 1 << 6;

    WAIT_BSY();

    /* check EOP */
    if (!(FLASH_SR & 0x00000020)) {
        /* disable option byte erase */
        FLASH_CR &= ~(1 << 5);
        return false;
    }
    /* reset EOP */
    FLASH_SR |= 1 << 5;

    /* disable option byte erase */
    FLASH_CR &= ~(1 << 5);

    return true;
}

/* assign write protection option bytes */
static void flash_assign_wrp(uint8_t page, uint16_t *wrp)
{
    if (page >= 0 && page < 16) {
        wrp[0] &= ~(1 << (page >> 1));
    } else if (page >= 16 && page < 32) {
        wrp[1] &= ~(1 << ((page - 16) >> 1));
    } else if (page >= 32 && page < 48) {
        wrp[2] &= ~(1 << ((page - 32) >> 1));
    } else if (page >= 48 && page < 62) {
        wrp[3] &= ~(1 << ((page - 48) >> 1));
    } else { /* 62 <= page < 256 */
        wrp[3] &= ~(1 << 7);
    }
}

/* perform write protection operation */
bool flash_write_protection(uint8_t *page, uint8_t page_num)
{
    if (!flash_opt_erase()) {
        return false;
    }

    /* enable OPTPG */
    FLASH_CR |= 1 << 4;

    /* default option bytes */
    struct opt_bytes opt_bytes = {
        .rdp = 0xaa,
        .user = 0xff,
        .data = {0xff, 0xff},
        .wrp = {0xff, 0xff, 0xff, 0xff},
    };

    for (uint8_t i = 0; i < page_num; i++) {
        flash_assign_wrp(page[i], opt_bytes.wrp);
    }

    uint16_t *flash_opt_byte = (uint16_t *) FLASH_OPT_BYTES_BASE;
    uint16_t *user_opt_byte = (uint16_t *) &opt_bytes;
    /* assign option bytes */
    for (uint8_t i = 0; i < 8; i++) {
        /* write the data */
        flash_opt_byte[i] = user_opt_byte[i];

        WAIT_BSY();

        /* check EOP */
        if (!(FLASH_SR & 0x00000020)) {
            /* disable OPTWRE and OPTPG */
            FLASH_CR &= ~(1 << 4) | ~(1 << 9);
            return false;
        }
        /* reset EOP */
        FLASH_SR |= 1 << 5;
    }

    /* disable OPTWRE and OPTPG */
    FLASH_CR &= ~(1 << 4) | ~(1 << 9);
    return true;
}

/* check flash operation is on */
bool flash_is_on(void)
{
    return FLASH_SR & (0x00000001);
}