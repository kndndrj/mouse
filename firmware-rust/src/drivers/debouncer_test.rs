#[cfg(test)]
mod tests {
    use crate::drivers::debouncer;

    fn get_debouncer(iterations: u8, initial_state: bool) -> debounce::Debouncer {
        let mut d = debounce::Debouncer::new(iterations);
        d.update(initial_state).unwrap();
        return d;
    }

    #[test]
    fn test_high_to_low() {
        let mut debouncer = get_debouncer(3, true);
        assert_eq!(debouncer.update(false).unwrap(), debounce::State::None);
        assert_eq!(debouncer.update(false).unwrap(), debounce::State::None);
        assert_eq!(debouncer.update(false).unwrap(), debounce::State::Low);
    }

    #[test]
    fn test_low_to_high() {
        let mut debouncer = get_debouncer(3, false);
        assert_eq!(debouncer.update(true).unwrap(), debounce::State::None);
        assert_eq!(debouncer.update(true).unwrap(), debounce::State::None);
        assert_eq!(debouncer.update(true).unwrap(), debounce::State::High);
    }

    #[test]
    fn test_with_noise_before() {
        let mut debouncer = get_debouncer(3, false);

        let mut pin_state: bool;
        for i in 0..20 {
            if i % 2 == 1 {
                pin_state = false;
            } else {
                pin_state = true;
            }
            debouncer.update(pin_state).unwrap();
        }

        assert_eq!(debouncer.update(true).unwrap(), debounce::State::None);
        assert_eq!(debouncer.update(true).unwrap(), debounce::State::High);
        assert_eq!(debouncer.update(true).unwrap(), debounce::State::High);
    }
}
