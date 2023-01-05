# Mouse Firmware Library

Building blocks for writing your own firmware. It uses "interfaces"
defined by embedded-hal crate to abstract away the hardware. If you want
to use this in your firmware, simply provide the required parameters
(usually in form of trait implementation) and you are good to go.
example:

```rust
// ...init code

// init the parts that implement required traits
let gpioa = p.GPIOA.split(&mut rcc);
let (
    spi_cs,
    spi_sck,
    spi_miso,
    spi_mosi,
    pmw_reset,
) = cortex_m::interrupt::free(move |cs| {
    (
        // SPI pins
        gpioa.pa4.into_push_pull_output(cs),
        gpioa.pa5.into_alternate_af0(cs),
        gpioa.pa6.into_alternate_af0(cs),
        gpioa.pa7.into_alternate_af0(cs),
        // PMW3360 reset pin
        gpioa.pa10.into_push_pull_output(cs),
    )
});
let spi = Spi::spi1(
    p.SPI1,
    (spi_sck, spi_miso, spi_mosi),
    MODE,
    1.mhz(),
    &mut rcc,
);

// finally use that to create a new instance of the driver
let mut pmw = Pmw3360::new(spi, spi_cs, pmw_reset, delay);
// ...
```
