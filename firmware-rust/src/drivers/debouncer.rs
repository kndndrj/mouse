#[cfg(test)]
mod debouncer_test;

use core::convert::Infallible;

#[derive(PartialEq, Debug)]
pub enum State {
    None,
    Low,
    High,
}
impl Default for State {
    fn default() -> Self {
        State::None
    }
}

// state is history of 1 and 0 reads...
// example: 0b010100111
// len_mask is a mask to retrieve only relevant bits from state...
// example:
//      len_mask: 0b00011111
//      state becomes: 0b00000111
// match_mask is a mask to check if state is debounced...
// example:
//      match_mask: 0b00001111
//      in this case state doesn't match the exact pattern.
// rev_match_mask is a reverse of match_mask...
// example:
//      rev_match_mask: 0b00010000
pub struct Debouncer {
    state: u16,
    len_mask: u16,
    match_mask: u16,
    rev_match_mask: u16,
}

impl Debouncer {
    pub fn new(iterations: u8) -> Self {
        let mmask: u16 = (1 << iterations) - 1;
        let lmask: u16 = (1 << (iterations + 1)) - 1;
        Self {
            state: 0,
            len_mask: lmask,
            match_mask: mmask,
            rev_match_mask: !mmask & lmask,
        }
    }

    pub fn update(&mut self, input: bool) -> Result<State, Infallible> {
        self.state = ((self.state << 1) | (input as u16)) & self.len_mask;

        if self.state == self.match_mask {
            return Ok(State::High);
        } else if self.state == self.rev_match_mask {
            return Ok(State::Low);
        }

        Ok(State::None)
    }
}
