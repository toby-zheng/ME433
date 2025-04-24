#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

unsigned int start, end;
float fps = 0;
uint16_t adc_result;
float converted;

void default_led_init();
void default_led_toggle();
void write_ascii(uint8_t x, uint8_t y, char c);
void write_string(uint8_t x, uint8_t y, char* str);

int main()
{
    stdio_init_all();
    default_led_init();

    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    while (true) {
        start = to_us_since_boot(get_absolute_time());
        // heartbeat
        default_led_toggle();

        // adc calc
        adc_result = adc_read();
        converted = (float) adc_result * 3.3/4095.0;

        ssd1306_clear();
        int i = 15;
        char msg[100];
        char fps_msg[50];
        sprintf(msg, "ADC: %.4f V", converted);
        write_string(20, 10, msg);
        sprintf(fps_msg, "%.2f", fps);
        write_string(20, 20, fps_msg);
        ssd1306_update();

        sleep_ms(1000);

        end = to_us_since_boot(get_absolute_time());

        fps = (float) 1.0/(float) ((end - start) * 1e-6);

    }
}

void default_led_init() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void default_led_toggle() {
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
}

void write_ascii(uint8_t x, uint8_t y, char c) {
    // step through the matrix
    for (uint8_t col = 0; col < 5; col++) {
        char column = ASCII[c - 0x20][col];
        for (uint8_t row = 0; row < 8; row++) {
            uint8_t pixel_state = (column >> row) & 0x1;
            ssd1306_drawPixel(x + col, y + row, pixel_state);
        }
    }
}

void write_string(uint8_t x, uint8_t y, char* str) {
    // null char breaks the loop
    while (*str) {
        write_ascii(x, y, *str++);
        // 1px padding
        x += (5 + 1);
    }
}