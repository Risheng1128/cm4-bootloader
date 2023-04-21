# toolchain
TOOLCHAIN = arm-none-eabi-
export CC = $(TOOLCHAIN)gcc
export SZ = $(TOOLCHAIN)size
export COPY = $(TOOLCHAIN)objcopy
export DUMP = $(TOOLCHAIN)objdump