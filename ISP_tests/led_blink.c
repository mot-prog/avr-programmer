// Clignote les 3 LEDS
#include <avr/io.h>
#include <util/delay.h>
#define BLINK_DELAY 50 // en milli secondes

int main()
{
  // configurer les led en sortie :
  DDRB |= (1 << 5); // PORTB 5e bit pour la LED1
  DDRB |= (1 << 6); // PORTB 6e bit pour la LED2
  DDRB |= (1 << 7); // PORTB 7e bit pour la LED3

  for (int i = 0; i < 50; i++)
  {
    // led on
    PORTB |= (1 << 5); // OR
    PORTB |= (1 << 6);
    PORTB |= (1 << 7);
    _delay_ms(BLINK_DELAY);
    // led off
    PORTB &= ~(1 << 5); // AND + NOT
    PORTB &= ~(1 << 6);
    PORTB &= ~(1 << 7);
    _delay_ms(BLINK_DELAY);
  }
  return 0;
}
