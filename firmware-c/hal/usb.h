#ifndef HAL_USB_H
#define HAL_USB_H

//
// Functions
//

void USB_init(void);

void USB_poll(void);

void USB_write_packet(const void *buf);

#endif
