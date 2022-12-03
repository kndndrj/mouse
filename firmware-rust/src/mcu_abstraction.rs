
pub trait SpiBus<Width> {
    type Error;
    // TODO: do something about clock polarity and clock edge
    // Send one word/byte and return the returned word/byte
    fn xfer(&mut self, data: Width) -> Result<Width, Self::Error>;
}

pub trait GpioOutputPin {
    type Error;
    // Set pin
    fn set(&mut self) -> Result<(), Self::Error>;
    // Clear pin
    fn clear(&mut self) -> Result<(), Self::Error>;
}

pub trait GpioInputPin {
    type Error;
    // Read pin status
    fn read(&self) -> Result<bool, Self::Error>;
}
