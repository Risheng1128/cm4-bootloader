/**
 * @file  flash.c
 * @brief functions about flash implementation
 */

#include "flash.h"

#define FLASH_UNLOCK_KEY1 0x45670123U
#define FLASH_UNLOCK_KEY2 0xCDEF89ABU

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