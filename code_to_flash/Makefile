CC = avr-gcc
OBJCOPY = avr-objcopy
MCU = atmega328p
F_CPU = 16000000UL
CFLAGS = -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)

HEX_DIR = ../hex
SRCS = $(wildcard *.c)
HEX_FILES = $(patsubst %.c,$(HEX_DIR)/%.hex,$(SRCS))

all: $(HEX_DIR) $(HEX_FILES)

$(HEX_DIR):
	mkdir -p $(HEX_DIR)

$(HEX_DIR)/%.hex: %.c
	@echo "Compilation of $< for $(MCU)..."
	$(CC) $(CFLAGS) -c $< -o $*.o
	$(CC) -mmcu=$(MCU) $*.o -o $*.elf
	$(OBJCOPY) -O ihex -R .eeprom $*.elf $@
	@rm -f $*.o $*.elf
	@echo "Sucess : $@ generated."

clean:
	rm -rf $(HEX_DIR) *.o *.elf