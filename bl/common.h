/**
 * @file  common.h
 * @brief This file contains common macros
 */
#pragma once

#define UNUSED __attribute__((unused))

/* MCU device ID code */
#define DBGMCU *(volatile uint32_t *) 0xE0042000U
