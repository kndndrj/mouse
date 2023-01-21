#![no_std]
#![no_main]

use panic_halt as _;

use packed_struct::prelude::*;

use stm32f0xx_hal::spi::{Mode, Phase, Polarity, Spi};
use stm32f0xx_hal::{delay::Delay, pac, prelude::*};

use usbd_human_interface_device::device::mouse::WheelMouseReport;

#[cfg(not(feature = "disable_usb"))]
use {
    embedded_time::duration::Milliseconds,
    stm32f0xx_hal::usb::{Peripheral, UsbBus},
    usb_device::prelude::*,
    usbd_human_interface_device::{
        device::mouse::WHEEL_MOUSE_REPORT_DESCRIPTOR,
        prelude::*,
        {
            hid_class::{prelude::InterfaceProtocol, UsbPacketSize},
            interface::raw::RawInterfaceBuilder,
        },
    },
};

use cortex_m;
use cortex_m_rt::entry;

use mouse_libs::{
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

    // Delay
    let delay = Delay::new(cp.SYST, &rcc);

    // Sensor
    let mut pmw = Pmw3360::new(spi, spi_cs, pmw_reset, delay);
    pmw.power_up().ok();
    pmw.set_cpi(1200).unwrap_or_default();

    // Encoder
    let mut enc = Encoder::new(enc_a, enc_b);

    // Debouncer for cpi pin
    let mut debouncer_cpi = Debouncer::new(8);

    // USB
    // NOTE: keep usb last before loop -> might not enumerate fast enough
    #[cfg(not(feature = "disable_usb"))]
    let usb_bus = UsbBus::new(Peripheral {
        usb: p.USB,
        pin_dm: usb_dm,
        pin_dp: usb_dp,
    });
    #[cfg(not(feature = "disable_usb"))]
    let mut mouse = UsbHidClassBuilder::new()
        .add_interface(
            RawInterfaceBuilder::new(WHEEL_MOUSE_REPORT_DESCRIPTOR)
                .boot_device(InterfaceProtocol::Mouse)
                .description("Wheel Mouse")
                .in_endpoint(UsbPacketSize::Bytes8, Milliseconds(1))
                .unwrap()
                .without_out_endpoint()
                .build(),
        )
        .build(&usb_bus);
    // USB_SERIAL_NUMBER defined in build.rs at compile time
    #[cfg(not(feature = "disable_usb"))]
    let mut usb_dev = UsbDeviceBuilder::new(&usb_bus, UsbVidPid(0xc410, 0x0000))
        .manufacturer("kndndrj")
        .product("mouse")
        .serial_number(env!("USB_SERIAL_NUMBER"))
        .build();

    let mut report = WheelMouseReport::default();

    loop {
        #[cfg(not(feature = "disable_usb"))]
        {

            if !usb_dev.poll(&mut [&mut mouse]) {
                continue;
            }

            mouse.interface().write_report(&report.pack().unwrap()).ok();
        }

        report = WheelMouseReport::default();

        let motion_data = pmw.burst_read().unwrap_or_default();

        if motion_data.motion && motion_data.on_surface {
            if motion_data.dx > 127 {
                report.x = -127;
            } else if motion_data.dx < -127 {
                report.x = 127;
            } else {
                report.x = -motion_data.dx as i8;
            }

            if motion_data.dy > 127 {
                report.y = -127;
            } else if motion_data.dy < -127 {
                report.y = 127;
            } else {
                report.y = -motion_data.dy as i8;
            }
        }

        enc.update().ok();

        if report.vertical_wheel == 0 {
            report.vertical_wheel = enc.read().unwrap_or_default().count();
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
    }
}
