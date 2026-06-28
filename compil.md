## blink
```bash
avr-gcc -mmcu=atmega16u2 -Wall -Werror -I. -DF_CPU=16000000 -Os -o led_blink.elf led_blink.c && avr-objcopy -j .text -j .data -O ihex led_blink.elf led_blink.hex && avrdude -c avrisp -p atmega16u2 -P /dev/ttyACM0 -b 19200 -U flash:w:led_blink.hex
```
## Boutons
```bash
avr-gcc -mmcu=atmega16u2 -Wall -Werror -I. -DF_CPU=16000000 -Os -o boutons.elf boutons.c && avr-objcopy -j .text -j .data -O ihex boutons.elf boutons.hex && avrdude -c avrisp -p atmega16u2 -P /dev/ttyACM0 -b 19200 -U flash:w:boutons.hex
```
