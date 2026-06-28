CC = avr-gcc
OBJCOPY = avr-objcopy
TARGET = blink
# Fréquence d'horloge par défaut (16 MHz pour un Arduino standard)
F_CPU = 16000000UL

CFLAGS = -Os -DF_CPU=$(F_CPU)

default: atmega328p
# 1. Cible pour l'Arduino Uno
atmega328p: MCU = atmega328p
atmega328p: clean compile
# 2. Cible pour un ATtiny85 
attiny85: MCU = attiny85
attiny85: F_CPU = 8000000UL
attiny85: clean compile
# 3. Cible pour ton programmateur
atmega16u2: MCU = atmega16u2
atmega16u2: clean compile

compile: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	mv $@ ../$@

$(TARGET).elf: $(TARGET).o
	$(CC) -mmcu=$(MCU) $< -o $@

$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -mmcu=$(MCU) -c $< -o $@

clean:
	@echo "Nettoyage des anciens fichiers de compilation..."
	rm -f *.o *.elf *.hex ../$(TARGET).hex

.PHONY: default atmega328p attiny85 atmega16u2 compile clean