#include <stdio.h>
#include <stdlib.h>
#include "isp_ids.h"

#define EP_OUT 0x01
#define EP_IN 0x82

void isp_ids(libusb_device_handle *dev_handle, unsigned char data_in[3])
{
    // ----OUT----
    unsigned char data_out = 0x01; // octet à envoyer pout l'ISP (choix)
    int actual_length;

    int r = libusb_interrupt_transfer(dev_handle, EP_OUT, &data_out, 1, &actual_length, 50);
    if (r == 0)
    {
        printf("[PC] Ordre envoyé : \033[33m0x%02X\033[0m\n", data_out);
    }
    else
    {
        printf("\033[31m[PC] Erreur OUT : %s\033[0m\n", libusb_error_name(r));
    }

    // ----IN ----

    printf("[PC] Attente de la réponse (Purge des anciens paquets)...\n");

    // Boucle pour ignorer les paquets de 1 octet (les boutons)
    // On boucle tant que le transfert réussit ET que la taille est de 1
    do
    {
        r = libusb_interrupt_transfer(dev_handle, EP_IN, data_in, 3, &actual_length, 100);
    } while (r == 0 && actual_length == 1);

    // Sortie de boucle : on a soit une erreur, soit notre paquet de 3 octets !
    if (r == 0 && actual_length == 3)
    {
        printf("\033[32mSignature reçue avec succès : \033[33m0x%02X 0x%02X 0x%02X\033[0m\n", data_in[0], data_in[1], data_in[2]);
        if (data_in[0] == 0x1E)
        {
            printf("\033[32m-> Fabricant : Atmel reconnu !\033[0m\n");
        }
    }
    else
    {
        printf("\033[31mErreur ou Timeout (reçu \033[33m%d\033[31m octets) : %s\033[0m\n", actual_length, libusb_error_name(r));
    }
}