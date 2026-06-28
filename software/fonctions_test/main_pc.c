#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //live clavier
#include <termios.h> //live clavier
#include <fcntl.h>   //live clavier
#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x03EB
#define PRODUCT_ID 0x2044
#define EP_OUT 0x01
#define EP_IN 0x82

libusb_device_handle *init_usb(void);
int reclamer_interface(libusb_device_handle *dev_handle);
int kbhit(void); // Fonction pour lire le clavier sans bloquer

int main()
{
    libusb_device_handle *dev_handle = init_usb();
    if (dev_handle == NULL)
        return 1;

    if (reclamer_interface(dev_handle) < 0)
        return 1;

    printf("\n=== MODE LIVE ACTIVÉ ===\n");
    printf("Touches :\n");
    printf(" [E] : Basculer LED 1 (PB5)\n");
    printf(" [Z] : Basculer LED 2 (PB6)\n");
    printf(" [A] : Basculer LED 3 (PB7)\n");
    printf(" [Q] : Quitter\n");
    printf("========================\n\n");

    unsigned char data_out = 0x00;
    unsigned char last_data_in = 0xFF; // Pour mémoriser l'ancien état des boutons
    int actual_length;
    int running = 1;

    while (running)
    {
        // 1. --- LECTURE DU CLAVIER PC ---
        if (kbhit())
        {
            char c = getchar();
            int send_update = 0;

            switch (c)
            {
            case 'e':
                data_out ^= (1 << 5);
                send_update = 1;
                break; // Bascule PB5
            case 'z':
                data_out ^= (1 << 6);
                send_update = 1;
                break; // Bascule PB6
            case 'a':
                data_out ^= (1 << 7);
                send_update = 1;
                break; // Bascule PB7
            case 'q':
                running = 0;
                break;
            }

            if (send_update)
            {
                // Envoi de la nouvelle configuration OUT
                int r = libusb_interrupt_transfer(dev_handle, EP_OUT, &data_out, 1, &actual_length, 50);
                if (r == 0)
                {
                    printf("[PC] Ordre envoyé : 0x%02X\n", data_out);
                }
                else
                {
                    printf("[PC] Erreur OUT : %s\n", libusb_error_name(r));
                }
            }
        }

        // 2. --- LECTURE DES BOUTONS (USB IN) ---
        unsigned char data_in = 0;

        // On met un timeout très court (10ms) pour ne pas bloquer la boucle
        int r = libusb_interrupt_transfer(dev_handle, EP_IN, &data_in, 1, &actual_length, 10);

        if (r == 0 && actual_length == 1)
        {
            // On n'affiche que si l'état des boutons a changé (pour éviter de spammer la console)
            if (data_in != last_data_in)
            {
                printf("[CARTE] État PORTC : 0x%02X -> ", data_in);
                if ((data_in & (1 << 6)) == 0)
                    printf("B6 pressé ! ");
                if ((data_in & (1 << 7)) == 0)
                    printf("B7 pressé ! ");
                if ((data_in & (1 << 6)) != 0 && (data_in & (1 << 7)) != 0)
                    printf("Relâchés.");
                printf("\n");

                last_data_in = data_in;
            }
        }

        // Petite pause pour ne pas surcharger le processeur du PC (10 millisecondes)
        usleep(10000);
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
        printf("Impossible d'ouvrir le périphérique (03EB:2044). Avez-vous utilisé sudo ?\n");
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

// Fonction système Linux pour détecter l'appui d'une touche sans bloquer
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO); // Désactive le mode canonique et l'écho
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}