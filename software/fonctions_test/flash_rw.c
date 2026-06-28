#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x03EB
#define PRODUCT_ID 0x2044
#define EP_OUT 0x01
#define EP_IN 0x82

libusb_device_handle *init_usb(void);
int reclamer_interface(libusb_device_handle *dev_handle);

int main()
{
    libusb_device_handle *dev_handle = init_usb();
    if (dev_handle == NULL)
        return 1;
    if (reclamer_interface(dev_handle) < 0)
        return 1;

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
        printf("Puce effacée avec succès !\n");
    }
    else
    {
        printf("Erreur lors de l'effacement : %s\n", libusb_error_name(r));
    }

    // =========================================================
    // 1. TEST D'ÉCRITURE (Byte Mode)
    // =========================================================
    printf("\n=== ECRITURE DE LA FLASH (Test : 4 premiers mots) ===\n\n");
    unsigned char data_write[5]; // Paquet de 5 octets pour l'écriture

    // On boucle sur les 4 premières adresses de mots (0, 1, 2, 3)
    for (uint16_t addr = 0; addr < 5; addr++)
    {
        // On invente une donnée factice pour le test (ex: 0xA000, 0xA001...)
        uint16_t fake_data = 0xA000 + addr;

        // Préparation du paquet selon le protocole LUFA
        data_write[0] = 0x03;                    // Commande d'écriture
        data_write[1] = (addr >> 8) & 0xFF;      // add HIGH
        data_write[2] = addr & 0xFF;             // add LOW
        data_write[3] = fake_data & 0xFF;        // fake LOW
        data_write[4] = (fake_data >> 8) & 0xFF; // fake HIGH

        // Envoi de l'ordre d'écriture en OUT
        r = libusb_interrupt_transfer(dev_handle, EP_OUT, data_write, 5, &actual_length, 100);
        if (r == 0)
        {
            printf("Ordre d'écriture envoyé -> Adresse: 0x%04X | Donnée: 0x%04X\n", addr, fake_data);
        }
        else
        {
            printf("Erreur d'écriture à l'adresse 0x%04X : %s\n", addr, libusb_error_name(r));
        }
    }

    // =========================================================
    // 2. TEST DE LECTURE
    // =========================================================
    printf("\n=== LECTURE DE LA FLASH (Test : 64 premiers octets) ===\n\n");

    unsigned char data_out[3];
    unsigned char data_in[8]; // Tampon de 8 octets pour recevoir 4 mots

    // Boucle pour lire 32 mots (64 octets), par pas de 4 mots
    for (uint16_t addr = 0; addr < 32; addr += 4)
    {
        // Préparation de la commande OUT (0x02 + Addr_High + Addr_Low)
        data_out[0] = 0x02;
        data_out[1] = (addr >> 8) & 0xFF; // Poids fort de l'adresse
        data_out[2] = addr & 0xFF;        // Poids faible de l'adresse

        // Demande de lecture en OUT
        r = libusb_interrupt_transfer(dev_handle, EP_OUT, data_out, 3, &actual_length, 100);
        if (r != 0)
        {
            printf("Erreur lors de la demande pour l'adresse 0x%04X : %s\n", addr, libusb_error_name(r));
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
            printf("Adresse 0x%04X : ", addr);
            for (int i = 0; i < 8; i += 2)
            {
                printf("%04X ", (data_in[i + 1] << 8) | data_in[i]);
            }
            printf("\n");
        }
        else
        {
            printf("Erreur de lecture a l'adresse 0x%04X (reçu %d octets)\n", addr, actual_length);
        }
    }
    printf("\nFermeture du programme...\n");
    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(NULL);

    return 0;
}

libusb_device_handle *init_usb(void)
{
    libusb_device_handle *dev_handle = NULL;
    if (libusb_init(NULL) < 0)
    {
        printf("Erreur d'initialisation de libusb\n");
        return NULL;
    }

    dev_handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (dev_handle == NULL)
    {
        printf("Périphérique introuvable. sudo ?\n");
        libusb_exit(NULL);
        return NULL;
    }

    if (libusb_kernel_driver_active(dev_handle, 0) == 1)
    {
        libusb_detach_kernel_driver(dev_handle, 0);
    }
    return dev_handle;
}

int reclamer_interface(libusb_device_handle *dev_handle)
{
    int r = libusb_claim_interface(dev_handle, 0);
    if (r < 0)
    {
        printf("Erreur de réclamation de l'interface : %s\n", libusb_error_name(r));
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return -1;
    }
    return 0;
}