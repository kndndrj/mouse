# Mouse Firmware in Rust - Implementation

Firmware implementation for mouse-pcb.

## Flashing

1. Build

   ```sh
   cargo build --release # for production release
   # or
   cargo build --features="disable_usb" # for dev build without usb support - for debugging
   ```

2. "Extract" the binary

   ```sh
      arm-none-eabi-objcopy -O binary target/thumbv6m-none-eabi/release/mouse-fw-rust out.bin
   ```

3. Flash using your debug probe

   ```sh
   # example using st-link
   st-flash write out.bin 0x08000000
   ```

4. Debug (for development)
   
   Use openocd/st-util/... to set up the gdb server and connect to it using arm-none-eabi-gdb
