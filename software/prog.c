#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <string.h>
#include "dev_handle.h"
#include "isp_ids.h"
#include "flash_write.h"
#include "flash_read.h"

// Définition de la structure "Cible"
typedef struct
{
    unsigned char signature[3];
    const char *nom;
    uint32_t taille_flash; // Taille totale en octets
    uint16_t taille_page;  // Taille d'une page en octets
} CibleAVR;

// BDD exemple
const CibleAVR base_de_donnees[] = {
    {{0x1E, 0x95, 0x0F}, "ATmega328P", 32768, 128},
    {{0x1E, 0x94, 0x89}, "ATmega16U2", 16384, 128},
    {{0x1E, 0x93, 0x0B}, "ATtiny85", 8192, 64}};

#define NB_CIBLES (sizeof(base_de_donnees) / sizeof(CibleAVR))

// Fonction de recherche
const CibleAVR *recherche_cible(unsigned char sig[3])
{
    for (long unsigned int i = 0; i < NB_CIBLES; i++)
    {
        if (sig[0] == base_de_donnees[i].signature[0] &&
            sig[1] == base_de_donnees[i].signature[1] &&
            sig[2] == base_de_donnees[i].signature[2])
        {
            return &base_de_donnees[i];
        }
    }
    return NULL; // Puce inconnue
}

int main(int argc, char *argv[])
{
    // Initialisation unique
    libusb_device_handle *dev_handle = init_usb();
    if (dev_handle == NULL)
        return 1;
    if (reclamer_interface(dev_handle) < 0)
        return 1;

    // On récupère les infos de la cible si elle est connue.
    unsigned char sig[3] = {0, 0, 0};
    isp_ids(dev_handle, sig);
    const CibleAVR *cible = recherche_cible(sig);
    if (cible == NULL)
        printf("\033[31mLa cible est inconnue.\033[0m\n");
    else
        printf("La cible est une %s, elle a une flash de \033[33m%d\033[0m octets et des pages de \033[33m%d\033[0m octets.\n", cible->nom, cible->taille_flash, cible->taille_page);
    // commandes utilisateur
    if (argc == 1)
    {
        printf("\033[31mErreur : Vous devez préciser une action.\033[0m\n");
        printf("Usage : make read OU make write <fichier.hex>\n");
    }
    else if (strcmp(argv[1], "-read") == 0)
    {
        printf("\nLancement de la lecture...\n");
        flash_read(dev_handle, cible->taille_flash);
    }
    else if (strcmp(argv[1], "-write") == 0)
    {
        if (argc < 3)
        {
            printf("\033[31mErreur : Il manque le nom du fichier .hex !\033[0m\n");
        }
        else
        {
            FILE *file = fopen(argv[2], "r");
            if (file == NULL)
                printf("\033[31mErreur : Impossible d'ouvrir le fichier %s.\033[0m\n", argv[2]);
            else
            {
                printf("\nOuverture du fichier %s...\n", argv[2]);
                flash_write(dev_handle, file);
                printf("\033[32mÉcriture terminée avec succès !\033[0m\n");
                fclose(file);
            }
        }
    }
    // Fermeture unique et propre
    printf("\nFermeture globale du programme...\n");
    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(NULL);

    return 0;
}