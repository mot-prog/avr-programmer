#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>

/* Adresses des points d'accès */
#define DATA_OUT_EPADDR (ENDPOINT_DIR_OUT | 1)
#define DATA_IN_EPADDR (ENDPOINT_DIR_IN | 2)

/* Taille des paquets */
#define DATA_SIZE 8

/* Type Defines: */
typedef struct
{
	USB_Descriptor_Configuration_Header_t Config;

	// Notre interface personnalisée
	USB_Descriptor_Interface_t CustomInterface;
	USB_Descriptor_Endpoint_t DataOutEndpoint;
	USB_Descriptor_Endpoint_t DataInEndpoint;
} USB_Descriptor_Configuration_t;

enum StringDescriptors_t
{
	STRING_ID_Language = 0,
	STRING_ID_Manufacturer = 1,
	STRING_ID_Product = 2,
};

/* Function Prototypes: */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
									const uint16_t wIndex,
									const void **const DescriptorAddress)
	ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

#endif