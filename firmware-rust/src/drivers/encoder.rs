use core::convert::Infallible;

use embedded_hal::blocking::delay::DelayUs;
use embedded_hal::digital::v2::InputPin;

#[derive(Default, Clone, Copy)]
pub struct Rotation(i8);

#[allow(dead_code)]
impl Rotation {
    pub fn count(&self) -> i8 {
        return self.0;
    }

    pub fn cw_count(&self) -> u8 {
        if self.0 > 0 {
            return self.0 as u8;
        }
        return 0;
    }

    pub fn ccw_count(&self) -> u8 {
        if self.0 < 0 {
            return -self.0 as u8;
        }
        return 0;
    }
}

pub struct Sw<A: InputPin, B: InputPin, D: DelayUs<u32>> {
    a_pin: A,
    b_pin: B,
    delay: D,
    _last_a_state: bool,
    _unread_state: Rotation,
}

impl<A, B, D> Sw<A, B, D>
where
    A: InputPin,
    B: InputPin,
    D: DelayUs<u32>,
{
    pub fn new(channel_a_pin: A, channel_b_pin: B, delay: D) -> Self {
        let a_state = channel_a_pin.is_high().unwrap_or_default();

        Self {
            a_pin: channel_a_pin,
            b_pin: channel_b_pin,
            delay,
            _last_a_state: a_state,
            _unread_state: Rotation(0),
        }
    }

    pub fn update(&mut self) -> Result<(), Infallible> {
        // read current state
        let current_a_state = self.a_pin.is_high().unwrap_or_default();

        if current_a_state == self._last_a_state {
            self._last_a_state = current_a_state;
            return Ok(());
        }

        if self.b_pin.is_high().unwrap_or_default() != current_a_state {
            self._unread_state = Rotation(self._unread_state.0 + 1);
        } else {
            self._unread_state = Rotation(self._unread_state.0 - 1);
        }

        self._last_a_state = current_a_state;

        // TODO: adjust
        // Debounce delay 20ms
        self.delay.delay_us(20000);

        Ok(())
    }
    pub fn read(&mut self) -> Result<Rotation, Infallible> {
        let r = self._unread_state;
        self._unread_state = Rotation(0);
        Ok(r)
    }
}
