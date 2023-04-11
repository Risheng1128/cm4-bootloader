/**
 * @file  crc.c
 * @brief functions about CRC implementation
 */

#include <stdint.h>

/* AHB peripheral clock enable register */
#define RCC_AHBENR *(volatile uint32_t *) 0x40021014U

/* CRC data register */
#define CRC_DR *(volatile uint32_t *) 0x40023000U
/* CRC control register */
#define CRC_CR *(volatile uint32_t *) 0x40023008U
/* initial CRC value */
#define CRC_INIT *(volatile uint32_t *) 0x40023010U

/* CRC hardware reset */
void crc_reset(void)
{
    CRC_CR |= 1 << 0;
}

/* CRC initialization */
void crc_init(void)
{
    /* CRC RCC enable */
    RCC_AHBENR |= 1 << 6;
    /* reset CRC */
    crc_reset();
}

/* compute CRC value */
uint32_t crc_compute(uint32_t input)
{
    CRC_DR = input;
    return CRC_DR;
}
