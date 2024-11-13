#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include "usb_device.h"
#include "usbd_dfu.h"   // Include DFU class support for USB device

// Function to initialize the USB device for DFU mode
void MX_USB_Device_Init(void);

#endif /* USB_DEVICE_H */
