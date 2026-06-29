ifeq (write,$(firstword $(MAKECMDGOALS)))
  HEX_FILE := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(HEX_FILE):;@:)
endif

.PHONY: all firmware software examples clean read write

all: firmware software convert

firmware:
	$(MAKE) -C firmware

software:
	$(MAKE) -C software

convert:
	$(MAKE) -C code_to_flash

clean:
	$(MAKE) -C firmware clean
	$(MAKE) -C software clean
	$(MAKE) -C code_to_flash clean

read: software
	sudo ./software/build/prog -read

write: software
	sudo ./software/build/prog -write $(HEX_FILE)