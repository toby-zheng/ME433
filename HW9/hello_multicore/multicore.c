/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/multicore.h"

#define FLAG_VALUE 123
int input;
uint16_t adc_value;

void core1_entry() {

    multicore_fifo_push_blocking(FLAG_VALUE);

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 1!\n");
    else
        printf("Its all gone well on core 1!");

    while (1) {
        // read user input
        g = multicore_fifo_pop_blocking();
        
        switch (g) {
            case 0:
                // read adc from A0
                adc_value = adc_read();
                multicore_fifo_push_blocking(FLAG_VALUE);
                break;
            case 1:
                // turn on LED GP15
                gpio_put(15, 1);
                multicore_fifo_push_blocking(FLAG_VALUE);
                break;
            
            case 2:
                // turn off LED GP15
                gpio_put(15, 0);
                multicore_fifo_push_blocking(FLAG_VALUE);
                break;

            default:
                // invalid command
                break;
        }

    }
        
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()){
        ;
    }
    printf("Hello, multicore!\n");

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    gpio_init(15);
    gpio_set_dir(15, GPIO_OUT);

    /// \tag::setup_multicore[]

    multicore_launch_core1(core1_entry);

    // Wait for it to start up

    uint32_t g = multicore_fifo_pop_blocking();

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 0!\n");
    else {
        multicore_fifo_push_blocking(FLAG_VALUE);
        printf("It's all gone well on core 0!");
    }

    while (1) {
        printf("Enter a command: ");
        scanf("%d", &input);
        printf("\n");

        switch (input) {
            case 0:
                multicore_fifo_push_blocking(input);
                g = multicore_fifo_pop_blocking();
                if (g != FLAG_VALUE) {
                    printf("Error w/ ADC value\n");
                }
                else {
                    printf("ADC Value: %f V\n", (float)adc_value* 3.3/4096.0);
                }
                break;

            case 1:
            multicore_fifo_push_blocking(input);
            g = multicore_fifo_pop_blocking();
            if (g != FLAG_VALUE) {
                printf("Error turning on LED\n");
            }
            else {
                printf("LED turned on\n");
            }
                break;

            case 2:
            multicore_fifo_push_blocking(input);
            g = multicore_fifo_pop_blocking();
            if (g != FLAG_VALUE) {
                printf("Error turning off LED\n");
            }
            else {
                printf("LED turned off\n");
            }
                break;

            default:
                printf("Invalid command\n");
                break;
        }
    }

    /// \end::setup_multicore[]
}
