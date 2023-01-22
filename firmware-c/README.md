# Mouse Firmware in C

Note: Abandoned due to re-implementation in Rust. You might still find something useful when tinkering around here.

Set up the build environment with (order of crossfile flags is important):

```sh
meson setup --cross-file=crossfiles/main.ini --cross-file=crossfiles/stm32/f0.ini build
```

And then compile the code:

```sh
meson compile -C build
```
