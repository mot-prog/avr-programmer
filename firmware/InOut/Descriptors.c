#include "Descriptors.h"

const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
	{
		.Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

		.USBSpecification = VERSION_BCD(1, 1, 0),
		.Class = USB_CSCP_VendorSpecificClass,
		.SubClass = USB_CSCP_NoDeviceSubclass,
		.Protocol = USB_CSCP_NoDeviceProtocol,

		.Endpoint0Size = FIXED_CONTROL_ENDPOINT_SIZE,

		.VendorID = 0x03EB,
		.ProductID = 0x2044,
		.ReleaseNumber = VERSION_BCD(0, 0, 1),

		.ManufacturerStrIndex = STRING_ID_Manufacturer,
		.ProductStrIndex = STRING_ID_Product,
		.SerialNumStrIndex = USE_INTERNAL_SERIAL,

		.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
	{
		.Config =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

				.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
				.TotalInterfaces = 1,

				.ConfigurationNumber = 1,
				.ConfigurationStrIndex = NO_DESCRIPTOR,

				.ConfigAttributes = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),

				.MaxPowerConsumption = USB_CONFIG_POWER_MA(100)},

		.CustomInterface =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

				.InterfaceNumber = 0,
				.AlternateSetting = 0,

				.TotalEndpoints = 2,

				.Class = USB_CSCP_VendorSpecificClass,
				.SubClass = 0x00,
				.Protocol = 0x00,

				.InterfaceStrIndex = NO_DESCRIPTOR},

		.DataOutEndpoint =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

				.EndpointAddress = DATA_OUT_EPADDR,
				.Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
				.EndpointSize = DATA_SIZE,
				.PollingIntervalMS = 0x05},

		.DataInEndpoint =
			{
				.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

				.EndpointAddress = DATA_IN_EPADDR,
				.Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
				.EndpointSize = DATA_SIZE,
				.PollingIntervalMS = 0x05}};

const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"LUFA Library");
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"Carte Custom IN-OUT");

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
									const uint16_t wIndex,
									const void **const DescriptorAddress)
{
	const uint8_t DescriptorType = (wValue >> 8);
	const uint8_t DescriptorNumber = (wValue & 0xFF);

	const void *Address = NULL;
	uint16_t Size = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
	case DTYPE_Device:
		Address = &DeviceDescriptor;
		Size = sizeof(USB_Descriptor_Device_t);
		break;
	case DTYPE_Configuration:
		Address = &ConfigurationDescriptor;
		Size = sizeof(USB_Descriptor_Configuration_t);
		break;
	case DTYPE_String:
		switch (DescriptorNumber)
		{
		case STRING_ID_Language:
			Address = &LanguageString;
			Size = pgm_read_byte(&LanguageString.Header.Size);
			break;
		case STRING_ID_Manufacturer:
			Address = &ManufacturerString;
			Size = pgm_read_byte(&ManufacturerString.Header.Size);
			break;
		case STRING_ID_Product:
			Address = &ProductString;
			Size = pgm_read_byte(&ProductString.Header.Size);
			break;
		}

		break;
	}

	*DescriptorAddress = Address;
	return Size;
}