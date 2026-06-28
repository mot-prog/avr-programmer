#pragma once
#include <libusb-1.0/libusb.h>

void isp_ids(libusb_device_handle *dev_handle, unsigned char data_in[3]);