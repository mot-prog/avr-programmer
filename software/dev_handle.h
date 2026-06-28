#pragma once
#include <libusb-1.0/libusb.h>

libusb_device_handle *init_usb(void);
int reclamer_interface(libusb_device_handle *dev_handle);