/**
 * @file  usart.h
 * @brief this file contains macros about GPIO and USART
 */

#pragma once
#include <stdint.h>

/* USART initialization */
void usart_init(void);

/* USART reset */
void usart_reset(void);

/* send n-bytes data */
void usart_send_data(uint8_t *pTxBuffer, uint8_t len);

/* receive 1 byte data */
uint8_t usart_receive_data(void);
