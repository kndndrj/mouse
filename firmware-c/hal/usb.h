#ifndef HAL_USB_H
#define HAL_USB_H

typedef void *usb_driver_t;

//
// Functions
//

usb_driver_t USB_init(void);

void USB_poll(usb_driver_t driver);

void USB_write_packet(usb_driver_t driver, const void *buf);

#endif
