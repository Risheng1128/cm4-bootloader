OUT ?= build
BIN ?= boot-main

# c source file
C_SRC = usart.c \
	bootloader.c \
	flash.c \
	crc.c \
	boot-main.c

C_INC = -I.

# assembly file
ASM_SRC = startup.s

# linker script
LDSCRIPT = link.ld

include ../common.mk