#pragma once
#include <libusb-1.0/libusb.h>

void flash_write(libusb_device_handle *dev_handle, FILE *file);