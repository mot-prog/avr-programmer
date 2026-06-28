#include <stdio.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID 0x03EB
#define PRODUCT_ID 0x2044

libusb_device_handle *init_usb(void)
{
    libusb_device_handle *dev_handle = NULL;
    if (libusb_init(NULL) < 0)
    {
        printf("\033[31mErreur d'initialisation de libusb\033[0m\n");
        return NULL;
    }

    dev_handle = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);
    if (dev_handle == NULL)
    {
        printf("\033[31mPériphérique introuvable. sudo ?\033[0m\n");
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
        printf("\033[31mErreur de réclamation de l'interface : %s\033[0m\n", libusb_error_name(r));
        libusb_close(dev_handle);
        libusb_exit(NULL);
        return -1;
    }
    return 0;
}