# toolchain
TOOLCHAIN = arm-none-eabi-
CC = $(TOOLCHAIN)gcc
SZ = $(TOOLCHAIN)size

# toolchain options
CFLAGS = -mcpu=cortex-m4 \
    -mthumb \
	$(C_INC) \
	-O0 \
	-Wall \
	-g

LDFLAGS = $(MCU) \
    -specs=nano.specs \
	-T$(LDSCRIPT) \
	-lc \
	-lm \
	-lnosys

# object file
OBJS  = $(patsubst %.c, $(OUT)/%.o, $(notdir $(C_SRC)))
OBJS += $(patsubst %.s, $(OUT)/%.o, $(notdir $(ASM_SRC)))

all: $(OUT)/$(BIN)
# compile
$(OUT)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OUT)/%.o: %.s
	$(CC) -c $(CFLAGS) $< -o $@

# link
$(OUT)/$(BIN): $(OUT) $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	$(SZ) $@

$(OUT):
	mkdir $@

disassembly: $(OUT)/$(BIN)
	$(TOOLCHAIN)objdump -d $^ > $(OUT)/$(BIN).S

.PHONY: disassembly