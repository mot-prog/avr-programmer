#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include "Descriptors.h"
#include <LUFA/Drivers/USB/USB.h>

#define SPI_DDR DDRB
#define SPI_PORT PORTB
#define SPI_SS PB0
#define SPI_SCK PB1
#define SPI_MOSI PB2
#define SPI_MISO PB3

void EVENT_USB_Device_ConfigurationChanged(void);
void ProcessOUT(void);
void ProcessIN(void);
void spi_init(void);
void spi_ON(void);
void spi_OFF(void);
void ProcessIN_Flash(void);
uint8_t spi_transfer(uint8_t data);
void chip_erase(void);

uint8_t commande_PC = 0;
uint16_t flash_address = 0;

int main(void)
{
	/*Initialisation du matériel */
	// LEDS
	DDRB |= (1 << 5) | (1 << 6) | (1 << 7);
	PORTB &= ~((1 << 5) | (1 << 6) | (1 << 7)); // OFF
	// BUTTONS
	DDRC &= ~((1 << 6) | (1 << 7));
	PORTC |= (1 << 6) | (1 << 7);

	/*Initialisation de la pile USB LUFA */
	USB_Init();
	GlobalInterruptEnable();
	spi_init();

	for (;;)
	{
		USB_USBTask();

		// SÉCURITÉ : On ne fait rien si le périphérique n'est pas encore configuré par le PC
		if (USB_DeviceState != DEVICE_STATE_Configured)
			continue;

		ProcessOUT();
		ProcessIN();
	}
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
	// Configuration du point d'accès pour recevoir les ordres (LEDs)
	Endpoint_ConfigureEndpoint(DATA_OUT_EPADDR, EP_TYPE_INTERRUPT, DATA_SIZE, 1);
	// Configuration du point d'accès pour envoyer les données (Boutons/ISP)
	Endpoint_ConfigureEndpoint(DATA_IN_EPADDR, EP_TYPE_INTERRUPT, DATA_SIZE, 1);
}

void ProcessOUT(void)
{
	Endpoint_SelectEndpoint(DATA_OUT_EPADDR);
	if (Endpoint_IsOUTReceived())
	{
		if (Endpoint_IsReadWriteAllowed())
		{
			// On lit l'octet envoyé par le PC
			commande_PC = Endpoint_Read_8();

			if (commande_PC == 0x02) // Lecture flash (flash_read.c)
			{
				flash_address = ((uint16_t)Endpoint_Read_8() << 8); // Poids fort
				flash_address |= Endpoint_Read_8();					// Poids faible
			}

			else if (commande_PC == 0x03) // Ecriture de flash (flash_rw.c)
			{
				flash_address = ((uint16_t)Endpoint_Read_8() << 8); // Poids fort
				flash_address |= Endpoint_Read_8();					// Poids faible
				uint8_t low = Endpoint_Read_8();
				uint8_t high = Endpoint_Read_8();

				spi_ON();
				_delay_ms(5); // Temps de réveil cible

				// (3.2 datasheet) Enable Memory Access
				spi_transfer(0xAC);
				spi_transfer(0x53);
				spi_transfer(0x00);
				spi_transfer(0x00);

				// Low byte
				spi_transfer(0x40);
				spi_transfer(flash_address >> 8);
				spi_transfer(flash_address & 0xFF);
				spi_transfer(low);

				// High byte
				spi_transfer(0x48);
				spi_transfer(flash_address >> 8);
				spi_transfer(flash_address & 0xFF);
				spi_transfer(high);

				// Write
				spi_transfer(0x4C);
				spi_transfer(flash_address >> 8);
				spi_transfer(flash_address & 0xFF);
				spi_transfer(0x00);

				_delay_ms(5); // Attente de la gravure physique
				spi_OFF();
			}
			else if (commande_PC == 0x04)
			{
				chip_erase();
			}
			else if (commande_PC != 0x01)
			{
				PORTB = commande_PC; // Gestion des LEDs
			}
		}
		Endpoint_ClearOUT();
	}
}

void ProcessIN(void)
{
	Endpoint_SelectEndpoint(DATA_IN_EPADDR);
	if (Endpoint_IsReadWriteAllowed())
	{
		if (commande_PC == 0x02)
		{
			spi_ON();

			// (3.2 datasheet) Enable Memory Access
			spi_transfer(0xAC);
			spi_transfer(0x53);
			spi_transfer(0x00);
			spi_transfer(0x00);

			for (uint16_t i = 0; i < 4; i++)
			{
				uint16_t current_addr = flash_address + i;

				// Lecture du Low Byte
				spi_transfer(0x20);
				spi_transfer(current_addr >> 8);   // On décale de 8 les 2 octets donc on a 0x00HIGH = 0xHIGH
				spi_transfer(current_addr & 0xFF); // On fait un & à l'adresse avec 0x00FF donc on a 0x00LOW = 0xLOW
				uint8_t data_low = spi_transfer(0x00);

				// Lecture du High Byte
				spi_transfer(0x28);
				spi_transfer(current_addr >> 8);
				spi_transfer(current_addr & 0xFF);
				uint8_t data_hight = spi_transfer(0x00);

				// envoie au PC
				Endpoint_Write_8(data_low);
				Endpoint_Write_8(data_hight);
			}
			spi_OFF();
			commande_PC = 0; // On a fini d'envoyer le bloc
		}
		else if (commande_PC == 0x01) // isp_ids.c
		{
			{
				// Mode ISP : Le PC a demandé les identifiants
				spi_ON();

				// (3.2 datasheet) Enable Memory Access
				spi_transfer(0xAC);
				spi_transfer(0x53);
				spi_transfer(0x00);
				spi_transfer(0x00);

				// --- Lecture du 1er octet de signature (Adresse 0x00) ---
				spi_transfer(0x30);
				spi_transfer(0x00);
				spi_transfer(0x00);
				uint8_t data_1 = spi_transfer(0x00); // La cible répond pendant cet octet

				// --- Lecture du 2ème octet de signature (Adresse 0x01) ---
				spi_transfer(0x30);
				spi_transfer(0x00);
				spi_transfer(0x01);
				uint8_t data_2 = spi_transfer(0x00);

				// --- Lecture du 3ème octet de signature (Adresse 0x02) ---
				spi_transfer(0x30);
				spi_transfer(0x00);
				spi_transfer(0x02);
				uint8_t data_3 = spi_transfer(0x00);

				spi_OFF();

				// On envoie les 3 octets au PC
				Endpoint_Write_8(data_1);
				Endpoint_Write_8(data_2);
				Endpoint_Write_8(data_3);

				commande_PC = 0;
			}
		}
		// commandes boutons
		else
			Endpoint_Write_8(PINC & 0xC0);

		Endpoint_ClearIN();
	}
}

void spi_init(void)
{
	SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS); // Définition des sorties
	SPI_DDR &= ~(1 << SPI_MISO);								 // Définition de l'entrée
	SPI_PORT |= (1 << SPI_SS);									 // Désactivation du périphérique
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0); // Activation SPI (SPE) en état maître (MSTR)
	SPSR &= ~(1 << SPI2X);										 // horloge F_CPU/128 (SPI2X=0, SPR1=1,SPR0=1)
}

void spi_ON(void) // Activer le périphérique
{
	SPI_PORT &= ~(1 << SPI_SS); // Ligne SS à l'état bas
}

void spi_OFF(void) // Désactiver le périphérique
{
	SPI_PORT |= (1 << SPI_SS); // Ligne SS à l'état haut
}

uint8_t spi_transfer(uint8_t data)
{
	SPDR = data; // Octet a envoyer
	while (!(SPSR & (1 << SPIF)))
		; // Attente fin envoi (drapeau SPIF du statut)
	return SPDR;
}

void chip_erase(void)
{
	spi_ON();
	_delay_ms(5);
	// (3.2 datasheet) Enable Memory Access
	spi_transfer(0xAC);
	spi_transfer(0x53);
	spi_transfer(0x00);
	spi_transfer(0x00);
	// (3.4.3 datasheet) erase
	spi_transfer(0xAC);
	spi_transfer(0x80);
	spi_transfer(0x00);
	spi_transfer(0x00);
	_delay_ms(5);
	spi_OFF();
}