#include <stdio.h>
#include <stdlib.h>
#include "flash_read.h"

#define EP_OUT 0x01
#define EP_IN 0x82

int flash_read(libusb_device_handle *dev_handle, uint32_t size)
{
    printf("\n=== LECTURE DE LA FLASH ===\n\n");

    int r;
    int actual_length;
    unsigned char data_out[3];
    unsigned char data_in[8]; // Tampon de 8 octets pour recevoir 4 mots

    // Boucle pour lire tout les mots de la flash(size/2 octets), par pas de 4 mots
    for (uint16_t addr = 0; addr < size / 2; addr += 4)
    {
        // Préparation de la commande OUT (0x02 + Addr_High + Addr_Low)
        data_out[0] = 0x02;
        data_out[1] = (addr >> 8) & 0xFF; // Poids fort de l'adresse
        data_out[2] = addr & 0xFF;        // Poids faible de l'adresse

        // Demande de lecture en OUT
        r = libusb_interrupt_transfer(dev_handle, EP_OUT, data_out, 3, &actual_length, 100);
        if (r != 0)
        {
            printf("\033[31mErreur lors de la demande pour l'adresse \033[33m0x%04X\033[31m : %s\033[0m\n", addr, libusb_error_name(r));
            continue;
        }

        // Réception IN (avec supression des paquets "boutons" d'1 octet)
        do
        {
            r = libusb_interrupt_transfer(dev_handle, EP_IN, data_in, 8, &actual_length, 100);
        } while (r == 0 && actual_length == 1); // Ignore les paquets de boutons

        // Affichage du bloc reçu
        if (r == 0 && actual_length == 8)
        {
            printf("Adresse \033[33m0x%04X\033[0m : ", addr);
            for (int i = 0; i < 8; i += 2)
            {
                printf("\033[33m%04X \033[0m", (data_in[i + 1] << 8) | data_in[i]);
            }
            printf("\n");
        }
        else
        {
            printf("\033[31mErreur de lecture a l'adresse \033[33m0x%04X\033[31m (reçu \033[33m%d\033[31m octets)\033[0m\n", addr, actual_length);
        }
    }

    printf("\nFermeture du programme...\n");
    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(NULL);

    return 0;
}