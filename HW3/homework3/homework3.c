#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main()
{
    // init
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);
    // on-board LED init
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    // button init
    gpio_init(2);
    gpio_set_dir(2, GPIO_IN);

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("Start!\n");
    // LED on
    gpio_put(25, true);
    
    // wait for button press
    while (gpio_get(2) == 1) {
        sleep_ms(100);
    }
    // LED off
    gpio_put(25, false);

    // main loop
    while (true) {
        int sample_num, count;
        uint16_t result;
        float converted_value;
        printf("How many samples to take (btwn. 1-100): \t");
        scanf("%d", &sample_num);
        printf("\r\n");

        // bounds for input
        if (sample_num < 1) {
            sample_num = 1;
        } else if (sample_num > 100) { 
            sample_num = 100;
        }
        
        for (count = 0; count < sample_num; count ++) {
            result = adc_read();
            converted_value = (float) result * 3.3/4095.0;
            printf("ADC Sample %d: %.4f V\r\n", (count + 1), converted_value);
            // 100 Hz
            sleep_ms(100);
        }
    }
}
