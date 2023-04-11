/**
 * @file  flash.h
 * @brief This file contains macros about flash
 */
#pragma once

#define FLASH_BASE 0x08000000U
/* application start address */
#define FLASH_PAGE_8_BASE 0x08004000U

/* vector table offset */
#define VECT_TAB_OFF 0x4000U

/* vector table offset register */
#define VTOR *(volatile uint32_t *) 0xE000ED18U
