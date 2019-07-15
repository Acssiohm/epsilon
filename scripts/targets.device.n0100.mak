.PHONY: %_flash
%_flash: $(BUILD_DIR)/%.dfu
	@echo "DFU     $@"
	@echo "INFO    About to flash your device. Please plug your device to your computer"
	@echo "        using an USB cable and press the RESET button the back of your device."
	until $(PYTHON) scripts/device/dfu.py -l | grep -E "0483:a291|0483:df11" > /dev/null 2>&1; do sleep 2;done
	@echo "DFU     $@"
	$(Q) $(PYTHON) scripts/device/dfu.py -m -u $<
