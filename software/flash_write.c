#include <stdio.h>
#include <stdlib.h>
#include "flash_write.h"

#define EP_OUT 0x01
#define EP_IN 0x82

void flash_write(libusb_device_handle *dev_handle, FILE *file)
{
    int r;
    int actual_length;
    // =========================================================
    // 0. EFFACEMENT DE LA CIBLE (Chip Erase)
    // =========================================================
    printf("\n=== EFFACEMENT DE LA PUCE ===\n");
    unsigned char cmd_erase = 0x04;

    r = libusb_interrupt_transfer(dev_handle, EP_OUT, &cmd_erase, 1, &actual_length, 500);
    if (r == 0)
    {
        printf("\033[32mPuce effacée avec succès !\033[0m\n");
    }
    else
    {
        printf("\033[31mErreur lors de l'effacement : %s\033[0m\n", libusb_error_name(r));
    }

    // =========================================================
    // 1. TEST D'ÉCRITURE (Byte Mode)
    // =========================================================
    printf("\n=== ECRITURE DE LA FLASH ===\n\n");
    unsigned char data_write[5]; // Paquet de 5 octets pour l'écriture

    char line[256];
    unsigned int length, addr_in, type_in;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        sscanf(line, ":%2x%4x%2x", &length, &addr_in, &type_in);
        uint16_t type = (uint16_t)type_in;
        if (type == 0x01)
            break;
        if (type == 0x00)
        {
            // Du côté du fichier .HEX l'adresse est codé octet par octet. Contrairement à l'AVR qui stock des adresses de mots. Donc pour avoir l'adresse du mot(2 octets) il suffit de diviser l'adresse in par 2.
            uint16_t current_word_addr = addr_in / 2;

            for (unsigned int i = 0; i < length; i += 2)
            {
                unsigned int word_data;
                // On décale le point de départ du sscanf de 9 (début des octets du code) puis on l'avance d'un facteur de 2 par rapport à i. Comme i aussi avance d'un facteur de 2 alors le tout avance d'un facteur de 4. Comme on lit 4 valeurs ascii à la fois (en effet 1 octet = 2 valeurs ascii => 2 octets = 1 mot = 4 valeurs ascii) le décallage est respécté.
                sscanf(line + 9 + (i * 2), "%4x", &word_data);
                uint16_t data = ((word_data & 0xFF) << 8) | (word_data >> 8);

                // Préparation du paquet selon le protocole LUFA
                data_write[0] = 0x03;                            // Commande d'écriture
                data_write[1] = (current_word_addr >> 8) & 0xFF; // add HIGH
                data_write[2] = current_word_addr & 0xFF;        // add LOW
                data_write[3] = data & 0xFF;                     // LOW
                data_write[4] = (data >> 8) & 0xFF;              // HIGH

                // Envoi de l'ordre d'écriture en OUT
                r = libusb_interrupt_transfer(dev_handle, EP_OUT, data_write, 5, &actual_length, 100);
                if (r == 0)
                {
                    printf("\033[32mOrdre d'écriture envoyé -> Adresse: \033[33m0x%04X\033[32m | Donnée: \033[33m0x%04X\033[0m\n", current_word_addr, data);
                }
                else
                {
                    printf("\033[31mErreur d'écriture à l'adresse \033[33m0x%04X\033[31m : %s\033[0m\n", current_word_addr, libusb_error_name(r));
                }
                current_word_addr++;
            }
        }
        else
            printf("\033[31mErreur de type\033[0m\n");
    }
}