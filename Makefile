# Generic Makefile
BL-BIN ?= boot-main
APP-BIN ?= app-main

OUT ?= $(CURDIR)/build
BIN ?= main
# serial port
PORT ?= /dev/ttyACM0

$(OUT)/$(BIN): $(OUT)/$(BL-BIN) $(OUT)/$(APP-BIN)
	cat $^ > $@

$(OUT)/$(BL-BIN):
	$(MAKE) -C bl OUT=$(OUT) BIN=$(BL-BIN)

$(OUT)/$(APP-BIN):
	$(MAKE) -C app OUT=$(OUT) BIN=$(APP-BIN)

host:
	python3 host/host.py -p $(PORT)

debug:
	openocd -f board/st_nucleo_f3.cfg

upload:
	openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c " program $(OUT)/$(BIN) verify exit "

clean:
	-@$(RM) -r $(OUT)

.PHONY: $(OUT)/$(BIN) $(OUT)/$(BL-BIN) $(OUT)/$(APP-BIN) host debug upload clean