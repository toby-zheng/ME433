#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   20     // can be any GPIO
#define PIN_SCK  18
#define PIN_MOSI 19

float sine_array[200];
float triangle_array[100]; 

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void wave_array_init();
void writeDAC(char dac_select, float voltage);

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    wave_array_init();
    
    int i = 0;
    int j = 0;
        while (true) {
            // 2 sine cycles per second, 1 triangleke
            printf("Hello, world!\n");
            writeDAC('A', sine_array[i]);
            writeDAC('B', triangle_array[j]);
            i++;
            j++;
            sleep_us(2500);
            writeDAC('A', sine_array[i]);
            i++;
            sleep_us(2500);
            writeDAC('A', sine_array[i]);
            i++;
            sleep_us(2500);
            writeDAC('A', sine_array[i]);
            i++;
            sleep_us(2500);
            if (i >= 200) i = 0;
            if (j >= 100) j = 0;
        }
    }
 
void wave_array_init() {
    for (int i = 0; i < 200; i++) {
        float t = (float) i / 200.0;
        sine_array[i] = 1.65 + 1.65*sinf(2 * 3.14159 * 1.0 * t);
    }

    for (int i = 0; i < 50; i++) {
        float value = 3.3 * (float) i / 49.0;
        triangle_array[i] = value;
        triangle_array[i + 50] = 3.3 - value;
    }
}

void writeDAC(char dac_select, float voltage) {
    uint16_t msg, voltage_bits;
    uint8_t data[2];
    char dac_select_bit;
    
    // DAC output select
    switch (dac_select) {
        case 'A':
            dac_select_bit = 0;
            break;
        case 'B':
            dac_select_bit = 1;
            break;
        default:
            printf("!!!Error in DAC Select bit\n");
            break;
    }
    
    // voltage conversion V to bits
    voltage_bits = (int) ((voltage/3.3) * 1024.0); 
    // clamping
    if (voltage_bits > 1023) voltage_bits = 1023;
    if (voltage_bits < 0) voltage_bits = 0;

    // bit manipulation --> data transfer
    msg = (dac_select_bit << 15) | (0b111 << 12) | (voltage_bits << 2);
    data[0] = (msg) >> 8;     // MS Byte
    data[1] = ((msg) & 0xFF);               
    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, 2);
    cs_deselect(PIN_CS);
}