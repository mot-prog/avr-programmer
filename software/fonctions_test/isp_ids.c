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

    // ----OUT----
    unsigned char data_out = 0x01; // octet à envoyer pout l'ISP (choix)
    int actual_length;

    int r = libusb_interrupt_transfer(dev_handle, EP_OUT, &data_out, 1, &actual_length, 50);
    if (r == 0)
    {
        printf("[PC] Ordre envoyé : 0x%02X\n", data_out);
    }
    else
    {
        printf("[PC] Erreur OUT : %s\n", libusb_error_name(r));
    }

    // ----IN ----
    unsigned char data_in[3] = {0, 0, 0};

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
        printf("Signature reçue avec succès : 0x%02X 0x%02X 0x%02X\n", data_in[0], data_in[1], data_in[2]);

        if (data_in[0] == 0x1E)
        {
            printf("-> Fabricant : Atmel reconnu !\n");
        }
    }
    else
    {
        printf("Erreur ou Timeout (reçu %d octets) : %s\n", actual_length, libusb_error_name(r));
    }

    // Fermeture propre
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
