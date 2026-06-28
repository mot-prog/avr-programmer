#include <avr/io.h>
#include <util/delay.h>

#define b7 0b10000000
#define b6 0b01000000

#define L5 0b11011111
#define L6 0b10111111
#define L7 0b01111111

void config()
{
    DDRB |= (1 << 5);
    DDRB |= (1 << 6);
    DDRB |= (1 << 7);

    DDRC &= 0x00;

    // Activation des résistances de Pull-Up internes sur le port C
    PORTC |= (b6 | b7);
}

int lire_bouton(int bouton)
{
    return (PINC & bouton) == 0;
}

void ecrire_LED(int LED, int etat)
{
    switch (etat)
    {
    case 0:
        PORTB &= LED; // Applique le masque avec le 0 pour éteindre
        break;
    case 1:
        PORTB |= ~LED; // Inverse le masque pour avoir un 1 et allumer
        break;
    }
}

int main()
{
    config();

    while (1)
    {
        if (lire_bouton(b6) && lire_bouton(b7))
        {
            ecrire_LED(L7, 1);
            ecrire_LED(L6, 0);
            ecrire_LED(L5, 0);
        }
        else if (lire_bouton(b7))
        {
            ecrire_LED(L5, 1);
        }
        else if (lire_bouton((b6)))
        {
            ecrire_LED(L6, 1);
        }
        else
        {
            // On éteint tout si aucun bouton n'est pressé
            ecrire_LED(L6, 0);
            ecrire_LED(L5, 0);
            ecrire_LED(L7, 0);
        }
    }
    return 0;
}