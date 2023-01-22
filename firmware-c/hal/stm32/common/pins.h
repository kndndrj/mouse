#ifndef HAL_PINS_H
#define HAL_PINS_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>

typedef struct port_pin {
  uint32_t port;
  uint32_t pin;
  uint32_t rcc;
} io_t;

#define PORT_PIN(port, pin, rcc) \
  (io_t) { port, pin, rcc }
#define PORT(port_pin) port_pin.port
#define PIN(port_pin) port_pin.pin
#define RCC_PORT(port_pin) port_pin.rcc

#define IO_A0 PORT_PIN(GPIOA, GPIO0, RCC_GPIOA)
#define IO_A1 PORT_PIN(GPIOA, GPIO1, RCC_GPIOA)
#define IO_A2 PORT_PIN(GPIOA, GPIO2, RCC_GPIOA)
#define IO_A3 PORT_PIN(GPIOA, GPIO3, RCC_GPIOA)
#define IO_A4 PORT_PIN(GPIOA, GPIO4, RCC_GPIOA)
#define IO_A5 PORT_PIN(GPIOA, GPIO5, RCC_GPIOA)
#define IO_A6 PORT_PIN(GPIOA, GPIO6, RCC_GPIOA)
#define IO_A7 PORT_PIN(GPIOA, GPIO7, RCC_GPIOA)
#define IO_A8 PORT_PIN(GPIOA, GPIO8, RCC_GPIOA)
#define IO_A9 PORT_PIN(GPIOA, GPIO9, RCC_GPIOA)
#define IO_A10 PORT_PIN(GPIOA, GPIO10, RCC_GPIOA)
#define IO_A11 PORT_PIN(GPIOA, GPIO11, RCC_GPIOA)
#define IO_A12 PORT_PIN(GPIOA, GPIO12, RCC_GPIOA)
#define IO_A13 PORT_PIN(GPIOA, GPIO13, RCC_GPIOA)
#define IO_A14 PORT_PIN(GPIOA, GPIO14, RCC_GPIOA)
#define IO_A15 PORT_PIN(GPIOA, GPIO15, RCC_GPIOA)

#define IO_B0 PORT_PIN(GPIOB, GPIO0, RCC_GPIOB)
#define IO_B1 PORT_PIN(GPIOB, GPIO1, RCC_GPIOB)
#define IO_B2 PORT_PIN(GPIOB, GPIO2, RCC_GPIOB)
#define IO_B3 PORT_PIN(GPIOB, GPIO3, RCC_GPIOB)
#define IO_B4 PORT_PIN(GPIOB, GPIO4, RCC_GPIOB)
#define IO_B5 PORT_PIN(GPIOB, GPIO5, RCC_GPIOB)
#define IO_B6 PORT_PIN(GPIOB, GPIO6, RCC_GPIOB)
#define IO_B7 PORT_PIN(GPIOB, GPIO7, RCC_GPIOB)
#define IO_B8 PORT_PIN(GPIOB, GPIO8, RCC_GPIOB)
#define IO_B9 PORT_PIN(GPIOB, GPIO9, RCC_GPIOB)
#define IO_B10 PORT_PIN(GPIOB, GPIO10, RCC_GPIOB)
#define IO_B11 PORT_PIN(GPIOB, GPIO11, RCC_GPIOB)
#define IO_B12 PORT_PIN(GPIOB, GPIO12, RCC_GPIOB)
#define IO_B13 PORT_PIN(GPIOB, GPIO13, RCC_GPIOB)
#define IO_B14 PORT_PIN(GPIOB, GPIO14, RCC_GPIOB)
#define IO_B15 PORT_PIN(GPIOB, GPIO15, RCC_GPIOB)

#define IO_C0 PORT_PIN(GPIOC, GPIO0, RCC_GPIOC)
#define IO_C1 PORT_PIN(GPIOC, GPIO1, RCC_GPIOC)
#define IO_C2 PORT_PIN(GPIOC, GPIO2, RCC_GPIOC)
#define IO_C3 PORT_PIN(GPIOC, GPIO3, RCC_GPIOC)
#define IO_C4 PORT_PIN(GPIOC, GPIO4, RCC_GPIOC)
#define IO_C5 PORT_PIN(GPIOC, GPIO5, RCC_GPIOC)
#define IO_C6 PORT_PIN(GPIOC, GPIO6, RCC_GPIOC)
#define IO_C7 PORT_PIN(GPIOC, GPIO7, RCC_GPIOC)
#define IO_C8 PORT_PIN(GPIOC, GPIO8, RCC_GPIOC)
#define IO_C9 PORT_PIN(GPIOC, GPIO9, RCC_GPIOC)
#define IO_C10 PORT_PIN(GPIOC, GPIO10, RCC_GPIOC)
#define IO_C11 PORT_PIN(GPIOC, GPIO11, RCC_GPIOC)
#define IO_C12 PORT_PIN(GPIOC, GPIO12, RCC_GPIOC)
#define IO_C13 PORT_PIN(GPIOC, GPIO13, RCC_GPIOC)
#define IO_C14 PORT_PIN(GPIOC, GPIO14, RCC_GPIOC)
#define IO_C15 PORT_PIN(GPIOC, GPIO15, RCC_GPIOC)

#define IO_D0 PORT_PIN(GPIOD, GPIO0, RCC_GPIOD)
#define IO_D1 PORT_PIN(GPIOD, GPIO1, RCC_GPIOD)
#define IO_D2 PORT_PIN(GPIOD, GPIO2, RCC_GPIOD)
#define IO_D3 PORT_PIN(GPIOD, GPIO3, RCC_GPIOD)
#define IO_D4 PORT_PIN(GPIOD, GPIO4, RCC_GPIOD)
#define IO_D5 PORT_PIN(GPIOD, GPIO5, RCC_GPIOD)
#define IO_D6 PORT_PIN(GPIOD, GPIO6, RCC_GPIOD)
#define IO_D7 PORT_PIN(GPIOD, GPIO7, RCC_GPIOD)
#define IO_D8 PORT_PIN(GPIOD, GPIO8, RCC_GPIOD)
#define IO_D9 PORT_PIN(GPIOD, GPIO9, RCC_GPIOD)
#define IO_D10 PORT_PIN(GPIOD, GPIO10, RCC_GPIOD)
#define IO_D11 PORT_PIN(GPIOD, GPIO11, RCC_GPIOD)
#define IO_D12 PORT_PIN(GPIOD, GPIO12, RCC_GPIOD)
#define IO_D13 PORT_PIN(GPIOD, GPIO13, RCC_GPIOD)
#define IO_D14 PORT_PIN(GPIOD, GPIO14, RCC_GPIOD)
#define IO_D15 PORT_PIN(GPIOD, GPIO15, RCC_GPIOD)

#define IO_E0 PORT_PIN(GPIOE, GPIO0, RCC_GPIOE)
#define IO_E1 PORT_PIN(GPIOE, GPIO1, RCC_GPIOE)
#define IO_E2 PORT_PIN(GPIOE, GPIO2, RCC_GPIOE)
#define IO_E3 PORT_PIN(GPIOE, GPIO3, RCC_GPIOE)
#define IO_E4 PORT_PIN(GPIOE, GPIO4, RCC_GPIOE)
#define IO_E5 PORT_PIN(GPIOE, GPIO5, RCC_GPIOE)
#define IO_E6 PORT_PIN(GPIOE, GPIO6, RCC_GPIOE)
#define IO_E7 PORT_PIN(GPIOE, GPIO7, RCC_GPIOE)
#define IO_E8 PORT_PIN(GPIOE, GPIO8, RCC_GPIOE)
#define IO_E9 PORT_PIN(GPIOE, GPIO9, RCC_GPIOE)
#define IO_E10 PORT_PIN(GPIOE, GPIO10, RCC_GPIOE)
#define IO_E11 PORT_PIN(GPIOE, GPIO11, RCC_GPIOE)
#define IO_E12 PORT_PIN(GPIOE, GPIO12, RCC_GPIOE)
#define IO_E13 PORT_PIN(GPIOE, GPIO13, RCC_GPIOE)
#define IO_E14 PORT_PIN(GPIOE, GPIO14, RCC_GPIOE)
#define IO_E15 PORT_PIN(GPIOE, GPIO15, RCC_GPIOE)

#define IO_F0 PORT_PIN(GPIOF, GPIO0, RCC_GPIOF)
#define IO_F1 PORT_PIN(GPIOF, GPIO1, RCC_GPIOF)
#define IO_F2 PORT_PIN(GPIOF, GPIO2, RCC_GPIOF)
#define IO_F3 PORT_PIN(GPIOF, GPIO3, RCC_GPIOF)
#define IO_F4 PORT_PIN(GPIOF, GPIO4, RCC_GPIOF)
#define IO_F5 PORT_PIN(GPIOF, GPIO5, RCC_GPIOF)
#define IO_F6 PORT_PIN(GPIOF, GPIO6, RCC_GPIOF)
#define IO_F7 PORT_PIN(GPIOF, GPIO7, RCC_GPIOF)
#define IO_F8 PORT_PIN(GPIOF, GPIO8, RCC_GPIOF)
#define IO_F9 PORT_PIN(GPIOF, GPIO9, RCC_GPIOF)
#define IO_F10 PORT_PIN(GPIOF, GPIO10, RCC_GPIOF)
#define IO_F11 PORT_PIN(GPIOF, GPIO11, RCC_GPIOF)
#define IO_F12 PORT_PIN(GPIOF, GPIO12, RCC_GPIOF)
#define IO_F13 PORT_PIN(GPIOF, GPIO13, RCC_GPIOF)
#define IO_F14 PORT_PIN(GPIOF, GPIO14, RCC_GPIOF)
#define IO_F15 PORT_PIN(GPIOF, GPIO15, RCC_GPIOF)

#define IO_G0 PORT_PIN(GPIOG, GPIO0, RCC_GPIOG)
#define IO_G1 PORT_PIN(GPIOG, GPIO1, RCC_GPIOG)
#define IO_G2 PORT_PIN(GPIOG, GPIO2, RCC_GPIOG)
#define IO_G3 PORT_PIN(GPIOG, GPIO3, RCC_GPIOG)
#define IO_G4 PORT_PIN(GPIOG, GPIO4, RCC_GPIOG)
#define IO_G5 PORT_PIN(GPIOG, GPIO5, RCC_GPIOG)
#define IO_G6 PORT_PIN(GPIOG, GPIO6, RCC_GPIOG)
#define IO_G7 PORT_PIN(GPIOG, GPIO7, RCC_GPIOG)
#define IO_G8 PORT_PIN(GPIOG, GPIO8, RCC_GPIOG)
#define IO_G9 PORT_PIN(GPIOG, GPIO9, RCC_GPIOG)
#define IO_G10 PORT_PIN(GPIOG, GPIO10, RCC_GPIOG)
#define IO_G11 PORT_PIN(GPIOG, GPIO11, RCC_GPIOG)
#define IO_G12 PORT_PIN(GPIOG, GPIO12, RCC_GPIOG)
#define IO_G13 PORT_PIN(GPIOG, GPIO13, RCC_GPIOG)
#define IO_G14 PORT_PIN(GPIOG, GPIO14, RCC_GPIOG)
#define IO_G15 PORT_PIN(GPIOG, GPIO15, RCC_GPIOG)

#define IO_H0 PORT_PIN(GPIOH, GPIO0, RCC_GPIOH)
#define IO_H1 PORT_PIN(GPIOH, GPIO1, RCC_GPIOH)
#define IO_H2 PORT_PIN(GPIOH, GPIO2, RCC_GPIOH)
#define IO_H3 PORT_PIN(GPIOH, GPIO3, RCC_GPIOH)
#define IO_H4 PORT_PIN(GPIOH, GPIO4, RCC_GPIOH)
#define IO_H5 PORT_PIN(GPIOH, GPIO5, RCC_GPIOH)
#define IO_H6 PORT_PIN(GPIOH, GPIO6, RCC_GPIOH)
#define IO_H7 PORT_PIN(GPIOH, GPIO7, RCC_GPIOH)
#define IO_H8 PORT_PIN(GPIOH, GPIO8, RCC_GPIOH)
#define IO_H9 PORT_PIN(GPIOH, GPIO9, RCC_GPIOH)
#define IO_H10 PORT_PIN(GPIOH, GPIO10, RCC_GPIOH)
#define IO_H11 PORT_PIN(GPIOH, GPIO11, RCC_GPIOH)
#define IO_H12 PORT_PIN(GPIOH, GPIO12, RCC_GPIOH)
#define IO_H13 PORT_PIN(GPIOH, GPIO13, RCC_GPIOH)
#define IO_H14 PORT_PIN(GPIOH, GPIO14, RCC_GPIOH)
#define IO_H15 PORT_PIN(GPIOH, GPIO15, RCC_GPIOH)

#define IO_I0 PORT_PIN(GPIOI, GPIO0, RCC_GPIOI)
#define IO_I1 PORT_PIN(GPIOI, GPIO1, RCC_GPIOI)
#define IO_I2 PORT_PIN(GPIOI, GPIO2, RCC_GPIOI)
#define IO_I3 PORT_PIN(GPIOI, GPIO3, RCC_GPIOI)
#define IO_I4 PORT_PIN(GPIOI, GPIO4, RCC_GPIOI)
#define IO_I5 PORT_PIN(GPIOI, GPIO5, RCC_GPIOI)
#define IO_I6 PORT_PIN(GPIOI, GPIO6, RCC_GPIOI)
#define IO_I7 PORT_PIN(GPIOI, GPIO7, RCC_GPIOI)
#define IO_I8 PORT_PIN(GPIOI, GPIO8, RCC_GPIOI)
#define IO_I9 PORT_PIN(GPIOI, GPIO9, RCC_GPIOI)
#define IO_I10 PORT_PIN(GPIOI, GPIO10, RCC_GPIOI)
#define IO_I11 PORT_PIN(GPIOI, GPIO11, RCC_GPIOI)
#define IO_I12 PORT_PIN(GPIOI, GPIO12, RCC_GPIOI)
#define IO_I13 PORT_PIN(GPIOI, GPIO13, RCC_GPIOI)
#define IO_I14 PORT_PIN(GPIOI, GPIO14, RCC_GPIOI)
#define IO_I15 PORT_PIN(GPIOI, GPIO15, RCC_GPIOI)

#define IO_J0 PORT_PIN(GPIOJ, GPIO0, RCC_GPIOJ)
#define IO_J1 PORT_PIN(GPIOJ, GPIO1, RCC_GPIOJ)
#define IO_J2 PORT_PIN(GPIOJ, GPIO2, RCC_GPIOJ)
#define IO_J3 PORT_PIN(GPIOJ, GPIO3, RCC_GPIOJ)
#define IO_J4 PORT_PIN(GPIOJ, GPIO4, RCC_GPIOJ)
#define IO_J5 PORT_PIN(GPIOJ, GPIO5, RCC_GPIOJ)
#define IO_J6 PORT_PIN(GPIOJ, GPIO6, RCC_GPIOJ)
#define IO_J7 PORT_PIN(GPIOJ, GPIO7, RCC_GPIOJ)
#define IO_J8 PORT_PIN(GPIOJ, GPIO8, RCC_GPIOJ)
#define IO_J9 PORT_PIN(GPIOJ, GPIO9, RCC_GPIOJ)
#define IO_J10 PORT_PIN(GPIOJ, GPIO10, RCC_GPIOJ)
#define IO_J11 PORT_PIN(GPIOJ, GPIO11, RCC_GPIOJ)
#define IO_J12 PORT_PIN(GPIOJ, GPIO12, RCC_GPIOJ)
#define IO_J13 PORT_PIN(GPIOJ, GPIO13, RCC_GPIOJ)
#define IO_J14 PORT_PIN(GPIOJ, GPIO14, RCC_GPIOJ)
#define IO_J15 PORT_PIN(GPIOJ, GPIO15, RCC_GPIOJ)

#define IO_K0 PORT_PIN(GPIOK, GPIO0, RCC_GPIOK)
#define IO_K1 PORT_PIN(GPIOK, GPIO1, RCC_GPIOK)
#define IO_K2 PORT_PIN(GPIOK, GPIO2, RCC_GPIOK)
#define IO_K3 PORT_PIN(GPIOK, GPIO3, RCC_GPIOK)
#define IO_K4 PORT_PIN(GPIOK, GPIO4, RCC_GPIOK)
#define IO_K5 PORT_PIN(GPIOK, GPIO5, RCC_GPIOK)
#define IO_K6 PORT_PIN(GPIOK, GPIO6, RCC_GPIOK)
#define IO_K7 PORT_PIN(GPIOK, GPIO7, RCC_GPIOK)
#define IO_K8 PORT_PIN(GPIOK, GPIO8, RCC_GPIOK)
#define IO_K9 PORT_PIN(GPIOK, GPIO9, RCC_GPIOK)
#define IO_K10 PORT_PIN(GPIOK, GPIO10, RCC_GPIOK)
#define IO_K11 PORT_PIN(GPIOK, GPIO11, RCC_GPIOK)
#define IO_K12 PORT_PIN(GPIOK, GPIO12, RCC_GPIOK)
#define IO_K13 PORT_PIN(GPIOK, GPIO13, RCC_GPIOK)
#define IO_K14 PORT_PIN(GPIOK, GPIO14, RCC_GPIOK)
#define IO_K15 PORT_PIN(GPIOK, GPIO15, RCC_GPIOK)

#endif