/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define GPIO_WATCH_PIN 2

// global vars
volatile bool led_on = true;
volatile int count = 0;

static char event_str[128];

void gpio_event_string(char *buf, uint32_t events);

// Example code from blink

// Perform initialisation
int pico_led_init(void) {
    #if defined(PICO_DEFAULT_LED_PIN)
        // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
        // so we can use normal GPIO functionality to turn the led on and off
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
        return PICO_OK;
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // For Pico W devices we need to initialise the driver etc
        return cyw43_arch_init();
    #endif
    }

// Turn the led on or off
void pico_set_led(bool led_on) {
    #if defined(PICO_DEFAULT_LED_PIN)
        // Just set the GPIO on or off
        gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // Ask the wifi "driver" to set the GPIO on or off
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    #endif
    }

void gpio_callback() {
    // toggle LED and iterate count
    led_on = !led_on;
    count ++;
    pico_set_led(led_on);
    printf("Button press Number: %d\n", count);
}


int main() {
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    stdio_init_all();

    printf("Program Start\n");
    gpio_init(GPIO_WATCH_PIN);
    // change to just falling edge, have button w/ pull up resistor
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // Wait forever
    while (1);
}



// static const char *gpio_irq_str[] = {
//         "LEVEL_LOW",  // 0x1
//         "LEVEL_HIGH", // 0x2
//         "EDGE_FALL",  // 0x4
//         "EDGE_RISE"   // 0x8
// };

// void gpio_event_string(char *buf, uint32_t events) {
//     for (uint i = 0; i < 4; i++) {
//         uint mask = (1 << i);
//         if (events & mask) {
//             // Copy this event string into the user string
//             const char *event_str = gpio_irq_str[i];
//             while (*event_str != '\0') {
//                 *buf++ = *event_str++;
//             }
//             events &= ~mask;

//             // If more events add ", "
//             if (events) {
//                 *buf++ = ',';
//                 *buf++ = ' ';
//             }
//         }
//     }
//     *buf++ = '\0';
// }
