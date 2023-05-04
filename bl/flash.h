/**
 * @file  flash.h
 * @brief This file contains macros about flash
 */
#pragma once
#include <stdbool.h>
#include <stdint.h>

#define FLASH_BASE 0x08000000U
/* application start address */
#define APP_BASE 0x08004000U
/* vector table offset */
#define VECT_TAB_OFF 0x4000U

/* vector table offset register */
#define VTOR *(volatile uint32_t *) 0xE000ED18U

/* enable lock */
void flash_set_lock(void);

/* check flash operation is on */
bool flash_is_on(void);

/* get protection level */
uint8_t flash_get_protection_level(void);

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
bool flash_write_protect(uint8_t *page, uint8_t page_num);

/* perform write protection operation */
bool flash_write_unprotect(void);
