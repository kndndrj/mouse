#![no_std]
#![no_main]

use panic_halt as _;

use stm32f0xx_hal::usb::{Peripheral, UsbBus, UsbBusType};
use stm32f0xx_hal::{pac, prelude::*};

use usbd_hid::descriptor::generator_prelude::*;
use usbd_hid::descriptor::MouseReport;
use usbd_hid::hid_class::HIDClass;

use usb_device::{bus, prelude::*};

use cortex_m_rt::entry;

#[entry]
fn main() -> ! {
    let mut p = pac::Peripherals::take().unwrap();

    let mut rcc = p
        .RCC
        .configure()
        .hsi48()
        .enable_crs(p.CRS)
        .sysclk(48.mhz())
        .pclk(24.mhz())
        .freeze(&mut p.FLASH);

    let gpioa = p.GPIOA.split(&mut rcc);

    let usb_dm = gpioa.pa11;
    let usb_dp = gpioa.pa12;

    let usb = Peripheral {
        usb: p.USB,
        pin_dm: usb_dm,
        pin_dp: usb_dp,
    };

    static mut USB_BUS: Option<bus::UsbBusAllocator<UsbBusType>> = None;

    let mut usb_dev;
    let mut usb_hid;

    unsafe {
        USB_BUS = Some(UsbBus::new(usb));

        usb_hid = Some(HIDClass::new(
            USB_BUS.as_ref().unwrap(),
            MouseReport::desc(),
            20,
        ));

        usb_dev = UsbDeviceBuilder::new(USB_BUS.as_ref().unwrap(), UsbVidPid(0xc410, 0x0000))
            .manufacturer("Fake company")
            .product("mouse")
            .serial_number("TEST")
            .device_class(0)
            .build();
    }

    loop {
        let report = MouseReport {
            x: -10,
            y: -3,
            pan: 0,
            wheel: 0,
            buttons: 0,
        };

        usb_hid.as_mut().map(|h| h.push_input(&report));

        usb_poll(&mut usb_dev, usb_hid.as_mut().unwrap());
    }
}

fn usb_poll<B: bus::UsbBus>(usb_dev: &mut UsbDevice<'static, B>, hid: &mut HIDClass<'static, B>) {
    if !usb_dev.poll(&mut [hid]) {
        return;
    }
}
