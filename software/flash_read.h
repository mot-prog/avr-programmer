#pragma once
#include <libusb-1.0/libusb.h>

int flash_read(libusb_device_handle *dev_handle, uint32_t size);