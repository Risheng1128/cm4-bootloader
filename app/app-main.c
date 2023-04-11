/**
 * @file  app-main.c
 * @brief applicarion main function
 */

#include <stdint.h>

#define RCC_AHBENR *(volatile uint32_t *) 0x40021014U

#define GPIOB 0x48000400U
/* GPIO port mode register */
#define GPIOB_MODER *(volatile uint32_t *) (GPIOB + 0x00)
/* GPIO port output data register */
#define GPIOB_ODR *(volatile uint32_t *) (GPIOB + 0x14)

#define DELAY()                          \
    do {                                 \
        for (int i = 0; i < 100000; i++) \
            ;                            \
    } while (0)

void gpio_init(void)
{
    /* GPIOB clock enable */
    RCC_AHBENR |= 1 << 18;

    /* PB7 output mode */
    GPIOB_MODER |= 1 << 14;
}

int main(void)
{
    gpio_init();
    while (1) {
        DELAY();
        /* toggle PB7 */
        GPIOB_ODR ^= (1 << 7);
    }
    return 0;
}
