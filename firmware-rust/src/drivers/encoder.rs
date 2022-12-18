use core::convert::Infallible;

use embedded_hal::blocking::delay::DelayUs;
use embedded_hal::digital::v2::InputPin;

#[derive(PartialEq)]
pub enum Rotation {
    None,
    Clockwise,
    CounterClockwise,
}

impl Default for Rotation {
    fn default() -> Self {
        Self::None
    }
}

#[derive(Default, PartialEq)]
struct State {
    a: bool,
    b: bool,
}

pub struct Sw<A: InputPin, B: InputPin, D: DelayUs<u32>> {
    a_pin: A,
    b_pin: B,
    delay: D,
    state: State,
}

impl<A, B, D> Sw<A, B, D>
where
    A: InputPin,
    B: InputPin,
    D: DelayUs<u32>,
{
    pub fn new(channel_a_pin: A, channel_b_pin: B, delay: D) -> Self {
        Self {
            a_pin: channel_a_pin,
            b_pin: channel_b_pin,
            delay,
            state: State::default(),
        }
    }

    pub fn read(&mut self) -> Result<Rotation, Infallible> {
        // read current state
        let mut new_state = State::default();
        if self.a_pin.is_high().unwrap_or_default() {
            new_state.a = true;
        }
        if self.b_pin.is_high().unwrap_or_default() {
            new_state.b = true;
        }

        if self.state == new_state {
            return Ok(Rotation::None);
        }

        // determine direction based on state
        // We only care about leading edges -> cw: a, c   ccw: d, b
        //            _____       _____       _____
        //           |     |     |     |     |     |
        // ch A  ____|     |_____|     |_____|     |____
        //
        //           :  :  :  :  :  :  :  :  :  :  :  :
        //      __       _____       _____       _____
        //        |     |     |     |     |     |     |
        // ch B   |_____|     |_____|     |_____|     |__
        //
        //           :  :  :  :  :  :  :  :  :  :  :  :
        // event     a  b  c  d  a  b  c  d  a  b  c  d

        // Match structure:
        // ({old_a, old_b}, {new_a, new_b})
        let ret = match (&self.state, &new_state) {
            // CW
            // a
            (State { a: false, b: false }, State { a: true, b: false }) => Rotation::Clockwise,
            // c
            (State { a: true, b: true }, State { a: false, b: true }) => Rotation::Clockwise,

            // CCW
            // d
            (State { a: false, b: false }, State { a: false, b: true }) => {
                Rotation::CounterClockwise
            }
            // b
            (State { a: true, b: true }, State { a: true, b: false }) => Rotation::CounterClockwise,

            // Default
            _ => Rotation::None,
        };

        // Update previous state
        self.state = new_state;

        // TODO: adjust
        // Debounce delay 20ms
        self.delay.delay_us(20000);

        Ok(ret)
    }
}
