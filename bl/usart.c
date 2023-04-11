/**
 * @file  usart.c
 * @brief functions about USART implementation
 * @attention
 * - PD8: USART3 Tx
 * - PD9: USART3 Rx
 * -------------- GPIO Set ----------------
 * - GPIOD_MODE:    Alternate function mode
 * - GPIOD_OSPEEDR: Low Speed
 * - GPIOD_PUPDR:   No Pull-up & Pull-down
 * - GPIOD_OTYPER:  Push-Pull
 * - GPIOD_AFRH:    AF7 (USART3 Tx/Rx)
 * -------------- USART Set ----------------
 * - FCLK:            8MHz
 * - USART3_Mode:     Tx/Rx Enable
 * - USART3_Parity:   Parity disable
 * - USART3_StopBits: 1
 * - USART3_WordLen:  8
 * - USART3_Baud:     38400
 * - USART3_HW_FLOW:  None
 * - Oversampling:    16
 */

#include <stdint.h>

#define DEFAULT_FCLK 8000000U
#define BAUDRATE_38400 38400U

/* AHB peripheral clock enable register */
#define RCC_AHBENR *(volatile uint32_t *) 0x40021014U
/* APB1 peripheral clock enable register */
#define RCC_APB1ENR *(volatile uint32_t *) 0x4002101CU

/* GPIO port mode register */
#define GPIOD_MODE *(volatile uint32_t *) 0x48000C00U
/* GPIO alternate function high register */
#define GPIOD_AFRH *(volatile uint32_t *) 0x48000C24U
/* control register 1 */
#define USART3_CR1 *(volatile uint32_t *) 0x40004800U
/* baudrate register */
#define USART3_BRR *(volatile uint32_t *) 0x4000480CU
/* Request register */
#define USART3_RQR *(volatile uint32_t *) 0x40004818U
/* Interrupt and status register */
#define USART3_ISR *(volatile uint32_t *) 0x4000481CU
/* Receive data register */
#define USART3_RDR *(volatile uint32_t *) 0x40004824U
/* Transmit data register */
#define USART3_TDR *(volatile uint32_t *) 0x40004828U

void usart_init(void)
{
    /***************** GPIO set *****************/
    /* GPIOD RCC enable */
    RCC_AHBENR |= 1 << 20;
    /* PD8 PD9 set alternate function mode */
    GPIOD_MODE &= ~(0xF << 16);
    GPIOD_MODE |= (0x2 << 16) | (0x2 << 18);
    /* PD8: USART3 Tx | PD9: USART3 Rx */
    GPIOD_AFRH &= ~(0xFF << 0);
    GPIOD_AFRH |= 0x77 << 0;

    /***************** USART Set *****************/
    /* USART RCC enable */
    RCC_APB1ENR |= 1 << 18;
    /* UE (USART Enable) set */
    USART3_CR1 |= 1 << 0;
    /* Enable Tx/Rx */
    USART3_CR1 |= (1 << 2) | (1 << 3);
    /* Baudrate setup */
    USART3_BRR = DEFAULT_FCLK / BAUDRATE_38400;
}

/* send n-bytes data */
void usart_send_data(uint8_t *pTxBuffer, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++) {
        /* waiting for the transmit data register empty (bit[7] TXE) */
        while (!(USART3_ISR & 0x80))
            ;
        /* send data */
        USART3_TDR = (*pTxBuffer++ & 0xFF);
        /* waiting for the transmit complete (bit[6] TC) */
        while (!(USART3_ISR & 0x40))
            ;
    }
}

/* receive 1 byte data */
uint8_t usart_receive_data(void)
{
    uint8_t data;
    /* waiting for the transmit data transmit to USART_RDR register
     * (USART_ISR bit[5] RXNE) */
    while (!(USART3_ISR & 0x20))
        ;
    /* receive data */
    data = USART3_RDR;
    /* clear RXNE by USARTx_RQR (bit[3] RXFRQ) */
    USART3_RQR |= 1 << 3;
    return data;
}
