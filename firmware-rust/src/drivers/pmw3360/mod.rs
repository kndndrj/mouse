use core::convert::Infallible;

use crate::mcu_abstraction::SpiBus;
use embedded_hal::blocking::delay::DelayUs;
use embedded_hal::digital::v2::OutputPin;

pub struct Pmw3360<S: SpiBus<u8>, G: OutputPin, D: DelayUs<u16>> {
    spi: S,
    cs_pin: G,
    delay: D,
}

impl<S, G, D> Pmw3360<S, G, D>
where
    S: SpiBus<u8>,
    G: OutputPin,
    D: DelayUs<u16>,
{
    pub fn new(spi: S, cs_pin: G, delay: D) -> Self {
        Self { spi, cs_pin, delay }
    }

    pub fn write(&mut self, data: u8) -> Result<u8, Infallible> {
        self.cs_pin.set_low().ok();
        self.delay.delay_us(100);

        self.spi.xfer(data).ok();

        self.delay.delay_us(160);

        let mut ret = 0;
        if let Ok(r) = self.spi.xfer(0) {
            ret = r;
        }

        self.delay.delay_us(100);
        self.cs_pin.set_high().ok();

        Ok(ret)
    }

    // fn read() -> Result<(), Infallible> {}

    // fn power_up() -> Result<(), Infallible> {}
}
