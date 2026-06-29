ifeq (write,$(firstword $(MAKECMDGOALS)))
  HEX_FILE := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(HEX_FILE):;@:)
endif

.PHONY: all firmware software convert prog clean read write

all: firmware software convert

firmware:
	$(MAKE) -C firmware

software:
	$(MAKE) -C software

convert:
	$(MAKE) -C code_to_flash

prog: firmware/InOut/main.hex
	sudo dfu-programmer atmega16u2 erase && sudo dfu-programmer atmega16u2 flash firmware/InOut/main.hex && sudo dfu-programmer atmega16u2 reset

minicom: firmware/VirtualSerial/VirtualSerial.hex
	sudo dfu-programmer atmega16u2 erase && sudo dfu-programmer atmega16u2 flash firmware/VirtualSerial/VirtualSerial.hex && sudo dfu-programmer atmega16u2 reset

clean: 
	$(MAKE) -C firmware clean
	$(MAKE) -C software clean
	$(MAKE) -C code_to_flash clean

read: software
	sudo ./software/build/prog -read

write: software
	sudo ./software/build/prog -write $(HEX_FILE)