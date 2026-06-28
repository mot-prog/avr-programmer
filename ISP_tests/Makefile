all: boutons.hex led_blink.hex
%.elf: %.c
	avr-gcc -mmcu=atmega16u2 -Wall -Werror -I. -DF_CPU=16000000 -Os -o $@ $<
	
%.hex: %.elf
	avr-objcopy -j .text -j .data -O ihex $< $@ 

.PHONY: clean

clean: 
	rm -f *.hex


flash_blink: led_blink.hex
	avrdude -c avrisp -p atmega16u2 -P /dev/ttyACM0 -b 19200 -U flash:w:led_blink.hex
flash_boutons: boutons.hex
	avrdude -c avrisp -p atmega16u2 -P /dev/ttyACM0 -b 19200 -U flash:w:boutons.hex