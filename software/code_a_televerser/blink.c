#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    // La LED de l'Arduino Uno est sur PB5 (Digital pin 13)
    DDRB |= (1 << PB5); // PB5 en sortie

    while (1)
    {
        PORTB ^= (1 << PB5); // Inverse l'état de la LED
        _delay_ms(500);
    }
    return 0;
}