/**
 * @file  flash.h
 * @brief This file contains macros about flash
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#define FLASH_BASE 0x08000000U
/* page size: 2kB */
#define FLASH_PAGE_SIZE 0x00000800U
/* application start address */
#define FLASH_PAGE_8_BASE 0x08004000U

/* vector table offset */
#define VECT_TAB_OFF 0x4000U

/* vector table offset register */
#define VTOR *(volatile uint32_t *) 0xE000ED18U

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

/* enable lock */
#define SET_LOCK() FLASH_CR |= 1 << 7

/* perform unlock sequence */
void flash_unlock_sequence(void);

/* perform option unlock sequence */
void flash_opt_unlock_sequence(void);

/* perform write operation */
bool flash_write(uint32_t base_addr, uint8_t *buffer, uint8_t len);

/* perform erase operation */
bool flash_erase(uint8_t page, uint8_t page_num);

/* perform mass erase operation */
bool flash_mass_erase(void);

/* perform write protection operation */
bool flash_write_protection(uint8_t *page, uint8_t page_num);

/* check flash operation is on */
bool flash_is_on(void);
