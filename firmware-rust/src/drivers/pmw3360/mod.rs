mod registers;
mod srom_liftoff;
mod srom_tracking;

use core::convert::Infallible;

use crate::mcu_abstraction::SpiBus;
use embedded_hal::blocking::delay::DelayUs;
use embedded_hal::digital::v2::OutputPin;

use registers as reg;

#[derive(Default)]
pub struct BurstData {
    pub motion: bool,
    pub on_surface: bool,
    pub dx: u16,
    pub dy: u16,
    pub surface_quality: u8,
    pub raw_data_sum: u8,
    pub max_raw_data: u8,
    pub min_raw_data: u8,
    pub shutter: u16,
}

pub struct Pmw3360<S: SpiBus<u8>, CS: OutputPin, RESET: OutputPin, D: DelayUs<u32>> {
    spi: S,
    cs_pin: CS,
    reset_pin: RESET,
    delay: D,
}

impl<S, CS, RESET, D> Pmw3360<S, CS, RESET, D>
where
    S: SpiBus<u8>,
    CS: OutputPin,
    RESET: OutputPin,
    D: DelayUs<u32>,
{
    pub fn new(spi: S, cs_pin: CS, reset_pin: RESET, delay: D) -> Self {
        Self {
            spi,
            cs_pin,
            reset_pin,
            delay,
        }
    }

    pub fn power_up(&mut self) -> Result<(), Infallible> {
        // TODO: propagate errors
        // sensor reset not active
        self.reset_pin.set_high().ok();

        // reset the spi bus on the sensor
        self.cs_pin.set_high().ok();
        self.delay.delay_us(50);
        self.cs_pin.set_low().ok();
        self.delay.delay_us(50);

        // Write to reset register
        self.write(reg::POWER_UP_RESET, 0x5A).ok();
        // 100 ms delay
        self.delay.delay_us(10000);

        // read registers 0x02 to 0x06 (and discard the data)
        self.read(reg::MOTION).ok();
        self.read(reg::DELTA_X_L).ok();
        self.read(reg::DELTA_X_H).ok();
        self.read(reg::DELTA_Y_L).ok();
        self.read(reg::DELTA_Y_H).ok();

        // upload the firmware
        self.upload_fw().ok();

        let is_valid_signature = self.check_signature().unwrap_or_else(|_| -> bool { false });

        // Write 0x00 (rest disable) to Config2 register for wired mouse or 0x20 for
        // wireless mouse design.
        self.write(reg::CONFIG_2, 0x00).ok();

        self.delay.delay_us(100);

        // TODO: do this outside
        self.set_cpi(0x32).ok();

        if is_valid_signature {
            return Ok(());
        };

        Ok(())
    }

    pub fn burst_read(&mut self) -> Result<BurstData, Infallible> {
        // TODO: propagate errors
        // Write any value to Motion_burst register
        self.write(reg::MOTION_BURST, 0x00).ok();

        // Lower NCS
        self.cs_pin.set_low().ok();
        // Send Motion_burst address
        self.spi.xfer(reg::MOTION_BURST).ok();

        // tSRAD_MOTBR
        self.delay.delay_us(35);

        // Read the 12 bytes of burst data
        let mut buf = [0u8; 12];
        for i in 0..buf.len() {
            buf[i] = self.spi.xfer(0x00).unwrap_or_else(|_| 0);
        }

        // Raise NCS
        self.cs_pin.set_high().ok();
        // tBEXIT
        self.delay.delay_us(1);

        // combine the register values
        let data = BurstData {
            motion: (buf[0] & 0x80) != 0,
            on_surface: (buf[0] & 0x08) == 0, // 0 if on surface / 1 if off surface
            dx: (buf[3] as u16) << 8 | (buf[2] as u16),
            dy: (buf[5] as u16) << 8 | (buf[4] as u16),
            surface_quality: buf[6],
            raw_data_sum: buf[7],
            max_raw_data: buf[8],
            min_raw_data: buf[9],
            shutter: (buf[11] as u16) << 8 | (buf[10] as u16),
        };

        Ok(data)
    }

    pub fn set_cpi(&mut self, cpi: u8) -> Result<(), Infallible> {
        self.write(reg::CONFIG_1, cpi).ok();
        Ok(())
    }

    pub fn check_signature(&mut self) -> Result<bool, Infallible> {
        // TODO: propagate errors

        let srom = self.read(reg::SROM_ID).unwrap_or_else(|_| -> u8 { 0 });
        let pid = self.read(reg::PRODUCT_ID).unwrap_or_else(|_| -> u8 { 0 });
        let ipid = self
            .read(reg::INVERSE_PRODUCT_ID)
            .unwrap_or_else(|_| -> u8 { 0 });

        // signature for SROM 0x04
        Ok(srom == 0x04 && pid == 0x42 && ipid == 0xBD)
    }

    #[allow(dead_code)]
    pub fn self_test(&mut self) -> Result<bool, Infallible> {
        self.write(reg::SROM_ENABLE, 0x15).ok();
        self.delay.delay_us(10000);

        let u = self
            .read(reg::DATA_OUT_UPPER)
            .unwrap_or_else(|_| -> u8 { 0 }); // should be 0xBE
        let l = self
            .read(reg::DATA_OUT_LOWER)
            .unwrap_or_else(|_| -> u8 { 0 }); // should be 0xEF

        Ok(u == 0xBE && l == 0xEF)
    }

    fn write(&mut self, address: u8, data: u8) -> Result<(), Infallible> {
        // TODO: propagate errors
        self.cs_pin.set_low().ok();
        self.delay.delay_us(100);

        // send adress of the register, with MSBit = 1 to indicate it's a write
        self.spi.xfer(address | 0x80).ok();
        // send data
        self.spi.xfer(data).ok();
        // tSCLK-NCS for write operation
        self.delay.delay_us(100 + 35);
        self.cs_pin.set_high().ok();

        // tSWW/tSWR (=120us) minus tSCLK-NCS. Could be
        // shortened, but is looks like a safe lower bound
        self.delay.delay_us(100);

        Ok(())
    }

    fn read(&mut self, address: u8) -> Result<u8, Infallible> {
        // TODO: propagate errors
        self.cs_pin.set_low().ok();
        self.delay.delay_us(100);

        // send adress of the register, with MSBit = 0 to indicate it's a read
        self.spi.xfer(address & 0x7f).ok();

        // tSRAD
        self.delay.delay_us(160);

        let mut ret = 0;
        if let Ok(r) = self.spi.xfer(0x00) {
            ret = r;
        }

        self.delay.delay_us(100);
        self.cs_pin.set_high().ok();

        //  tSRW/tSRR (=20us) minus tSCLK-NCS
        self.delay.delay_us(19);

        Ok(ret)
    }
    fn upload_fw(&mut self) -> Result<(), Infallible> {
        // TODO: propagate errors
        // Write 0 to Rest_En bit of Config2 register to disable Rest mode.
        self.write(reg::CONFIG_2, 0x00).ok();

        // write 0x1d in SROM_enable reg for initializing
        self.write(reg::SROM_ENABLE, 0x1d).ok();

        // wait for 10 ms
        self.delay.delay_us(10000);

        // write 0x18 to SROM_enable to start SROM download
        self.write(reg::SROM_ENABLE, 0x18).ok();

        // lower NCS
        self.cs_pin.set_low().ok();

        // first byte is address
        self.spi.xfer(reg::SROM_LOAD_BURST | 0x80).ok();
        self.delay.delay_us(15);

        // send the rest of the firmware
        for element in srom_tracking::FW.iter() {
            self.spi.xfer(*element).ok();
            self.delay.delay_us(15);
        }

        self.delay.delay_us(2);
        self.cs_pin.set_high().ok();
        self.delay.delay_us(200);
        Ok(())
    }
}
