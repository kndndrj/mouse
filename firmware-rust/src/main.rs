#![no_std]
#![no_main]

mod drivers;

use panic_halt as _;

use stm32f0xx_hal::spi::{Mode, Phase, Polarity, Spi};
use stm32f0xx_hal::{delay::Delay, pac, prelude::*};

use usbd_hid::descriptor::MouseReport;

#[cfg(not(feature = "disable_usb"))]
use {
    stm32f0xx_hal::usb::{Peripheral, UsbBus, UsbBusType},
    usb_device::{bus, prelude::*},
    usbd_hid::descriptor::generator_prelude::*,
    usbd_hid::hid_class::HIDClass,
};

use cortex_m;
use cortex_m_rt::entry;

use drivers::{
    debouncer::{self, Debouncer},
    encoder::Encoder,
    pmw3360::Pmw3360,
};

#[entry]
fn main() -> ! {
    let mut p = pac::Peripherals::take().unwrap();
    let cp = cortex_m::Peripherals::take().unwrap();

    // Clock
    let mut rcc = p
        .RCC
        .configure()
        .hsi48()
        .enable_crs(p.CRS)
        .sysclk(48.mhz())
        .pclk(24.mhz())
        .freeze(&mut p.FLASH);

    // Pin maps
    let gpioa = p.GPIOA.split(&mut rcc);
    let (
        button_left,
        button_right,
        button_middle,
        button_cpi,
        spi_cs,
        spi_sck,
        spi_miso,
        spi_mosi,
        pmw_reset,
        usb_dm,
        usb_dp,
    ) = cortex_m::interrupt::free(move |cs| {
        (
            // Button pins
            gpioa.pa0,
            gpioa.pa1,
            gpioa.pa2,
            gpioa.pa3,
            // SPI pins
            gpioa.pa4.into_push_pull_output(cs),
            gpioa.pa5.into_alternate_af0(cs),
            gpioa.pa6.into_alternate_af0(cs),
            gpioa.pa7.into_alternate_af0(cs),
            // PMW3360 reset pin
            gpioa.pa10.into_push_pull_output(cs),
            // USB pins
            gpioa.pa11,
            gpioa.pa12,
        )
    });

    let gpiob = p.GPIOB.split(&mut rcc);
    let (enc_a, enc_b) = cortex_m::interrupt::free(move |_| {
        (
            // Encoder pins
            gpiob.pb6, gpiob.pb7,
        )
    });

    // SPI
    const MODE: Mode = Mode {
        polarity: Polarity::IdleHigh,
        phase: Phase::CaptureOnSecondTransition,
    };
    let spi = Spi::spi1(
        p.SPI1,
        (spi_sck, spi_miso, spi_mosi),
        MODE,
        1.mhz(),
        &mut rcc,
    );

    // USB
    #[cfg(not(feature = "disable_usb"))]
    let mut usb_hid: Option<HIDClass<UsbBus<Peripheral>>>;
    #[cfg(not(feature = "disable_usb"))]
    let mut usb_dev: UsbDevice<UsbBus<Peripheral>>;

    #[cfg(not(feature = "disable_usb"))]
    {
        let usb = Peripheral {
            usb: p.USB,
            pin_dm: usb_dm,
            pin_dp: usb_dp,
        };

        static mut USB_BUS: Option<bus::UsbBusAllocator<UsbBusType>> = None;

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
    }

    // Delay
    let delay = Delay::new(cp.SYST, &rcc);

    // Sensor
    let mut pmw = Pmw3360::new(spi, spi_cs, pmw_reset, delay);
    pmw.power_up().ok();

    // Encoder
    let mut enc = Encoder::new(enc_a, enc_b);

    // Debouncer for cpi pin
    let mut debouncer_cpi = Debouncer::new(8);

    let mut report = MouseReport {
        x: 0,
        y: 0,
        pan: 0,
        wheel: 0,
        buttons: 0,
    };

    loop {
        let motion_data = pmw.burst_read().unwrap_or_default();

        // // TODO: make this pretty
        if motion_data.motion && motion_data.on_surface {
            if motion_data.dx > 32767 {
                report.x = (65535 - motion_data.dx) as i8;
            } else {
                report.x = -(motion_data.dx as i8);
            }
            if motion_data.dy > 32767 {
                report.y = (65535 - motion_data.dy) as i8;
            } else {
                report.y = -(motion_data.dy as i8);
            }
        }

        enc.update().ok();

        if report.wheel == 0 {
            report.wheel = enc.read().unwrap_or_default().count();
        }

        if button_left.is_low().unwrap_or_default() {
            report.buttons = report.buttons | (1 << 0);
        }
        if button_right.is_low().unwrap_or_default() {
            report.buttons = report.buttons | (1 << 1);
        }
        if button_middle.is_low().unwrap_or_default() {
            report.buttons = report.buttons | (1 << 2);
        }
        // Debouncer is meant to be used with a scheduler, but the buttons on our pcb are very well
        // hw debounced, so we use it to detect the edge
        let cpi_button_state = debouncer_cpi
            .update(button_cpi.is_high().unwrap_or_default())
            .unwrap_or_default();
        if cpi_button_state == debouncer::State::Low {
            let mut cpi = pmw.get_cpi().unwrap_or_default() + 2000;
            if cpi > 12000 {
                cpi = 0;
            }
            pmw.set_cpi(cpi).unwrap_or_default();
        }

        #[cfg(not(feature = "disable_usb"))]
        {
            usb_hid.as_mut().map(|h| h.push_input(&report));

            if usb_dev.poll(&mut [usb_hid.as_mut().unwrap()]) {
                report = MouseReport {
                    x: 0,
                    y: 0,
                    pan: 0,
                    wheel: 0,
                    buttons: 0,
                };
            }
        }
    }
}
