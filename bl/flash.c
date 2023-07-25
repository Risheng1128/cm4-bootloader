/**
 * @file  flash.c
 * @brief functions about flash implementation
 */

#include "flash.h"

/* flash key register */
#define FLASH_KEYR *(volatile uint32_t *) 0x40022004U
/* Flash option key register */
#define FLASH_OPTKEYR *(volatile uint32_t *) 0x40022008U
/* flash status register */
#define FLASH_SR *(volatile uint32_t *) 0x4002200CU
/* flash control register */
#define FLASH_CR *(volatile uint32_t *) 0x40022010U
/* flash address register */
#define FLASH_AR *(volatile uint32_t *) 0x40022014U
/* flash option byte register */
#define FLASH_OBR *(volatile uint32_t *) 0x4002201CU

/* programming */
#define FLASH_CR_PG (1 << 0)
/* page erase */
#define FLASH_CR_PER (1 << 1)
/* mass erase */
#define FLASH_CR_MER (1 << 2)
/* option byte programming */
#define FLASH_CR_OPTPG (1 << 4)
/* option byte erase */
#define FLASH_CR_OPTER (1 << 5)
/* start */
#define FLASH_CR_STRT (1 << 6)
/* lock */
#define FLASH_CR_LOCK (1 << 7)
/* option bytes write enable */
#define FLASH_CR_OPTWRE (1 << 9)
/* force option byte loading */
#define FLASH_CR_OBL_LAUNCH (1 << 13)

/* end of operation */
#define FLASH_SR_EOP (1 << 5)
/* busy */
#define FLASH_SR_BSY (1 << 0)

/* read protection Level status */
#define FLASH_OBR_RDPRT ((1 << 1) | (1 << 2))

/* page size: 2kB */
#define FLASH_PAGE_SIZE 0x00000800U
#define FLASH_UNLOCK_KEY1 0x45670123U
#define FLASH_UNLOCK_KEY2 0xCDEF89ABU
#define FLASH_OPT_BYTES_BASE 0x1FFFF800U

/* read protection level */
#define FLASH_RDP_LEVEL0 0xAA
#define FLASH_RDP_LEVEL1 0xBB /* except 0xAA and 0xCC */
#define FLASH_RDP_LEVEL2 0xCC

/* Option byte organization
 * address    | [31:24] | [23:16] | [15:8]  | [7:0]
 * 0x1FFFF800 |  nUser  |  User   |  nRDP   |  RDP
 * 0x1FFFF804 |  nData1 |  Data1  |  nData0 |  Data0
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
        while (FLASH_SR & FLASH_SR_BSY) \
            ;                           \
    } while (0)

/* check flash operation */
static bool check_eop(uint16_t clear_bits)
{
    /* check EOP */
    if (!(FLASH_SR & FLASH_SR_EOP)) {
        FLASH_CR &= ~clear_bits;
        return false;
    }
    /* reset EOP */
    FLASH_SR |= FLASH_SR_EOP;
    return true;
}

/* perform option bytes erase operation */
static bool opt_erase(void)
{
    /* check flash operation is on */
    if (flash_is_on()) {
        return false;
    }

    /* enable option byte erase */
    FLASH_CR |= FLASH_CR_OPTER;
    /* start erase operation */
    FLASH_CR |= FLASH_CR_STRT;

    /* wait flash operation complete */
    WAIT_BSY();

    /* check EOP */
    if (!check_eop(FLASH_CR_OPTER)) {
        return false;
    }
    /* disable option byte erase */
    FLASH_CR &= ~FLASH_CR_OPTER;
    return true;
}

/* assign write protection option bytes */
static void assign_wrp(uint8_t page, uint16_t *wrp)
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

/* read current option bytes */
static void read_opt_bytes(struct opt_bytes *opt_bytes)
{
    opt_bytes->rdp = *(uint16_t *) FLASH_OPT_BYTES_BASE;
    opt_bytes->user = *(uint16_t *) (FLASH_OPT_BYTES_BASE + 2);
    *(uint32_t *) opt_bytes->data = *(uint32_t *) (FLASH_OPT_BYTES_BASE + 4);
    *(uint32_t *) opt_bytes->wrp = *(uint32_t *) (FLASH_OPT_BYTES_BASE + 8);
    *(uint32_t *) (opt_bytes->wrp + 2) =
        *(uint32_t *) (FLASH_OPT_BYTES_BASE + 12);
}

/* write option bytes */
static bool write_opt_bytes(struct opt_bytes *opt_bytes, uint8_t len)
{
    uint16_t *flash_opt_byte = (uint16_t *) FLASH_OPT_BYTES_BASE;
    uint16_t *user_opt_byte = (uint16_t *) opt_bytes;
    /* assign option bytes */
    for (uint8_t i = 0; i < len; i++) {
        /* write the data */
        flash_opt_byte[i] = user_opt_byte[i];

        /* wait flash operation complete */
        WAIT_BSY();

        /* check EOP */
        if (!check_eop(FLASH_CR_OPTPG | FLASH_CR_OPTWRE)) {
            return false;
        }
    }
    return true;
}

/* enable lock */
void flash_set_lock(void)
{
    FLASH_CR |= FLASH_CR_LOCK;
}

/* check flash operation is on */
bool flash_is_on(void)
{
    return FLASH_SR & FLASH_SR_BSY;
}

/* get protection level */
uint8_t flash_get_protection_level(void)
{
    return (FLASH_OBR & FLASH_OBR_RDPRT) >> 1;
}

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
    FLASH_CR |= FLASH_CR_PG;

    uint16_t *addr = (uint16_t *) base_addr;
    for (uint8_t i = 0; i < len; i += 2) {
        /* write to flash (half-word) */
        *addr++ = *(uint16_t *) (buffer + i);

        /* wait flash operation complete */
        WAIT_BSY();

        /* check EOP */
        if (!check_eop(FLASH_CR_PG)) {
            return false;
        }
    }

    /* disable flash programming */
    FLASH_CR &= ~FLASH_CR_PG;
    return true;
}

/* perform erase operation */
bool flash_erase(uint8_t page, uint8_t page_num)
{
    /* enable page erase */
    FLASH_CR |= FLASH_CR_PER;

    for (uint8_t i = 0; i < page_num; i++) {
        /* set page number to erase */
        uint32_t addr = FLASH_BASE + (page + i) * FLASH_PAGE_SIZE;
        FLASH_AR = addr;

        /* start erase operation */
        FLASH_CR |= FLASH_CR_STRT;

        /* wait flash operation complete */
        WAIT_BSY();

        /* check EOP */
        if (!check_eop(FLASH_CR_PER)) {
            return false;
        }
    }

    /* disable page erase */
    FLASH_CR &= ~FLASH_CR_PER;
    return true;
}

/* perform mass erase operation */
bool flash_mass_erase(void)
{
    /* enable mass erase page */
    FLASH_CR |= FLASH_CR_MER;
    /* start erase operation */
    FLASH_CR |= FLASH_CR_STRT;

    /* wait flash operation complete */
    WAIT_BSY();

    /* check EOP */
    if (!check_eop(FLASH_CR_MER)) {
        return false;
    }

    /* disable mass erase page */
    FLASH_CR &= ~FLASH_CR_MER;
    return true;
}

/* perform write protection operation */
bool flash_write_protect(uint8_t *page, uint8_t page_num)
{
    struct opt_bytes opt_bytes;

    /* save current option bytes */
    read_opt_bytes(&opt_bytes);
    if (!opt_erase()) {
        return false;
    }

    /* enable OPTPG */
    FLASH_CR |= FLASH_CR_OPTPG;

    for (uint8_t i = 0; i < page_num; i++) {
        assign_wrp(page[i], opt_bytes.wrp);
    }

    /* write all option bytes */
    if (!write_opt_bytes(&opt_bytes, 8)) {
        return false;
    }

    /* disable OPTWRE and OPTPG */
    FLASH_CR &= ~(FLASH_CR_OPTPG | FLASH_CR_OPTWRE);
    return true;
}

/* perform write protection operation */
bool flash_write_unprotect(void)
{
    struct opt_bytes opt_bytes;

    /* save current option bytes */
    read_opt_bytes(&opt_bytes);
    /* erase option bytes */
    if (!opt_erase()) {
        return false;
    }

    /* enable OPTPG */
    FLASH_CR |= FLASH_CR_OPTPG;

    /* write half option bytes */
    if (!write_opt_bytes(&opt_bytes, 4)) {
        return false;
    }

    /* disable OPTWRE and OPTPG */
    FLASH_CR &= ~(FLASH_CR_OPTPG | FLASH_CR_OPTWRE);
    return true;
}

/* perform read protection operation */
bool flash_read_protect(uint8_t level)
{
    struct opt_bytes opt_bytes;
    uint16_t rdp_level[3] = {FLASH_RDP_LEVEL0, FLASH_RDP_LEVEL1,
                             FLASH_RDP_LEVEL2};

    /* save current option bytes */
    read_opt_bytes(&opt_bytes);
    if (!opt_erase()) {
        return false;
    }

    /* enable OPTPG */
    FLASH_CR |= FLASH_CR_OPTPG;

    /* write RDP */
    opt_bytes.rdp = rdp_level[level];

    /* write all option bytes */
    if (!write_opt_bytes(&opt_bytes, 8)) {
        return false;
    }

    /* disable OPTWRE and OPTPG */
    FLASH_CR &= ~(FLASH_CR_OPTPG | FLASH_CR_OPTWRE);
    return true;
}

/* perform force option byte loading */
void flash_reload_opt_bytes(void)
{
    FLASH_CR |= FLASH_CR_OBL_LAUNCH;
}
