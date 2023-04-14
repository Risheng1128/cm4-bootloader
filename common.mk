-include toolchain.mk

# toolchain options
CFLAGS = -mcpu=cortex-m4 \
         -mthumb \
         $(C_INC) \
         -O0 \
         -Wall \
         -g

LDFLAGS = -mcpu=cortex-m4 \
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
