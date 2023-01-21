mod registers;
mod srom_liftoff;
mod srom_tracking;

use core::convert::Infallible;

use embedded_hal::blocking::{delay::DelayUs, spi::Transfer};
use embedded_hal::digital::v2::OutputPin;

use registers as reg;

#[derive(Default)]
pub struct BurstData {
    pub motion: bool,
    pub on_surface: bool,
    pub dx: i16,
    pub dy: i16,
    pub surface_quality: u8,
    pub raw_data_sum: u8,
    pub max_raw_data: u8,
    pub min_raw_data: u8,
    pub shutter: u16,
}

pub struct Pmw3360<S: Transfer<u8>, CS: OutputPin, RESET: OutputPin, D: DelayUs<u32>> {
    spi: S,
    cs_pin: CS,
    reset_pin: RESET,
    delay: D,
    // rw_flag is set if any writes or reads were performed
    rw_flag: bool,
}

impl<S, CS, RESET, D> Pmw3360<S, CS, RESET, D>
where
    S: Transfer<u8>,
    CS: OutputPin,
    RESET: OutputPin,
    D: DelayUs<u32>,
{
    pub fn new(spi: S, cs_pin: CS, reset_pin: RESET, delay: D) -> Self {
        let mut new = Self {
            spi,
            cs_pin,
            reset_pin,
            delay,
            rw_flag: true,
        };

        new.power_up().unwrap();

        new
    }

    pub fn burst_read(&mut self) -> Result<BurstData, Infallible> {
        // TODO: propagate errors

        // Write any value to Motion_burst register
        // if any write occured before
        if self.rw_flag {
            self.write(reg::MOTION_BURST, 0x00).ok();
            self.rw_flag = false;
        }

        // Lower NCS
        self.cs_pin.set_low().ok();
        // Send Motion_burst address
        self.spi.transfer(&mut [reg::MOTION_BURST]).ok();

        // tSRAD_MOTBR
        self.delay.delay_us(35);

        // Read the 12 bytes of burst data
        let mut buf = [0u8; 12];
        for i in 0..buf.len() {
            buf[i] = *self
                .spi
                .transfer(&mut [0x00])
                .unwrap_or_else(|_| -> &[u8] { &[0] })
                .first()
                .unwrap();
        }

        // Raise NCS
        self.cs_pin.set_high().ok();
        // tBEXIT
        self.delay.delay_us(1);

        // combine the register values
        let data = BurstData {
            motion: (buf[0] & 0x80) != 0,
            on_surface: (buf[0] & 0x08) == 0, // 0 if on surface / 1 if off surface
            dx: (buf[3] as i16) << 8 | (buf[2] as i16),
            dy: (buf[5] as i16) << 8 | (buf[4] as i16),
            surface_quality: buf[6],
            raw_data_sum: buf[7],
            max_raw_data: buf[8],
            min_raw_data: buf[9],
            shutter: (buf[11] as u16) << 8 | (buf[10] as u16),
        };

        Ok(data)
    }

    pub fn set_cpi(&mut self, cpi: u16) -> Result<(), Infallible> {
        let val: u16;
        if cpi < 100 {
            val = 0
        } else if cpi > 12000 {
            val = 0x77
        } else {
            val = (cpi - 100) / 100;
        }
        self.write(reg::CONFIG_1, val as u8).ok();
        Ok(())
    }

    pub fn get_cpi(&mut self) -> Result<u16, Infallible> {
        let val = self.read(reg::CONFIG_1).unwrap_or_default() as u16;
        Ok((val + 1) * 100)
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
        // tNCS-SCLK
        self.delay.delay_us(1);

        // send adress of the register, with MSBit = 1 to indicate it's a write
        self.spi.transfer(&mut [address | 0x80]).ok();
        // send data
        self.spi.transfer(&mut [data]).ok();

        // tSCLK-NCS (write)
        self.delay.delay_us(35);
        self.cs_pin.set_high().ok();

        // tSWW/tSWR minus tSCLK-NCS (write)
        self.delay.delay_us(145);

        self.rw_flag = true;

        Ok(())
    }

    fn read(&mut self, address: u8) -> Result<u8, Infallible> {
        // TODO: propagate errors
        self.cs_pin.set_low().ok();
        // tNCS-SCLK
        self.delay.delay_us(1);

        // send adress of the register, with MSBit = 0 to indicate it's a read
        self.spi.transfer(&mut [address & 0x7f]).ok();

        // tSRAD
        self.delay.delay_us(160);

        let mut ret = 0;
        if let Ok(r) = self.spi.transfer(&mut [0x00]) {
            ret = *r.first().unwrap();
        }

        // tSCLK-NCS (read)
        self.delay.delay_us(1);
        self.cs_pin.set_high().ok();

        //  tSRW/tSRR minus tSCLK-NCS
        self.delay.delay_us(20);

        self.rw_flag = true;

        Ok(ret)
    }

    fn power_up(&mut self) -> Result<(), Infallible> {
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

        if is_valid_signature {
            return Ok(());
        };

        Ok(())
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
        self.spi.transfer(&mut [reg::SROM_LOAD_BURST | 0x80]).ok();
        self.delay.delay_us(15);

        // send the rest of the firmware
        for element in srom_tracking::FW.iter() {
            self.spi.transfer(&mut [*element]).ok();
            self.delay.delay_us(15);
        }

        self.delay.delay_us(2);
        self.cs_pin.set_high().ok();
        self.delay.delay_us(200);
        Ok(())
    }
}
