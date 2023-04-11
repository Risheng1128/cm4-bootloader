/**
 * @file  main.c
 * @brief bootloader main function
 */

#include "bootloader.h"
#include "crc.h"
#include "usart.h"

int main(void)
{
    /* USART initialization */
    usart_init();
    /* CRC initialization */
    crc_init();

    while (1) {
        /* get the bootloader command */
        bl_read_command();
    }
    return 0;
}
