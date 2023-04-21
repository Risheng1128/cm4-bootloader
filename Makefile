# Generic Makefile
-include toolchain.mk

BL-BIN ?= boot-main
APP-BIN ?= app-main

OUT ?= $(CURDIR)/build
# serial port
PORT ?= /dev/ttyACM0

BL-TARGET = $(OUT)/$(BL-BIN)
APP-TARGET = $(OUT)/$(APP-BIN)

all: $(BL-TARGET) $(APP-TARGET)

$(BL-TARGET):
	$(MAKE) -C bl OUT=$(OUT) BIN=$(BL-BIN)

$(APP-TARGET):
	$(MAKE) -C app OUT=$(OUT) BIN=$(APP-BIN)

# generate application binary file
bin: $(APP-TARGET)
	$(COPY) -O binary $^ $^.bin

host:
	python3 host/host.py -p $(PORT)

disassembly: $(BL-TARGET) $(APP-TARGET)
	$(DUMP) -d $^ > $(BL-TARGET).S
	$(DUMP) -d $(APP-TARGET) > $(APP-TARGET).S

debug:
	openocd -f board/st_nucleo_f3.cfg

upload:
        # burn the application first
	openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c " program $(APP-TARGET) verify exit "
	openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c " program $(BL-TARGET) verify exit reset "

clean:
	-@$(RM) -r $(OUT)

.PHONY: all $(BL-TARGET) $(APP-TARGET) bin host disassembly debug upload clean