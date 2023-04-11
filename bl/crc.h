/**
 * @file  crc.h
 * @brief This file contains macros about crc
 */

#pragma once
#include <stdint.h>

/* CRC initialization */
void crc_init(void);

/* compute CRC value */
uint32_t crc_compute(uint32_t input);

/* CRC reset */
void crc_reset(void);
