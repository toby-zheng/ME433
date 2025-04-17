#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
#define SPI_PORT spi0
#define PIN_MISO 16
#define DAC_CS   20
#define RAM_CS   14
#define PIN_SCK  18
#define PIN_MOSI 19

union FloatInt {
    float f;
    uint32_t i;
};

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop");
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop");
}

void writeDAC(char dac_select, float voltage);
void spi_ram_init();
void spi_write_float(uint16_t addr, float value);
float spi_read_float(uint16_t addr);

int main() {
    stdio_init_all();

    spi_init(SPI_PORT, 10000000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(DAC_CS, GPIO_FUNC_SIO);
    gpio_set_function(RAM_CS, GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(DAC_CS);
    gpio_init(RAM_CS);
    gpio_set_dir(DAC_CS, GPIO_OUT);
    gpio_put(DAC_CS, 1);
    gpio_set_dir(RAM_CS, GPIO_OUT);
    gpio_put(RAM_CS, 1);

    spi_ram_init();

    for (int i = 0; i < 1000; i++) {
        float t = (float)i / 1000.0f;
        float val = 1.65f + 1.65f * sinf(2 * M_PI * t);
        spi_write_float(i * 4, val);
    }

    int i = 0;
    while (true) {
        float val = spi_read_float(i * 4);
        writeDAC('A', val);
        sleep_ms(1);
        i = (i + 1) % 1000;
    }
}

void spi_ram_init() {
    cs_select(RAM_CS);
    uint8_t init[2] = {0x01, 0x40};  // Enable sequential mode
    spi_write_blocking(SPI_PORT, init, 2);
    cs_deselect(RAM_CS);
}

void spi_write_float(uint16_t addr, float val) {
    union FloatInt num;
    num.f = val;

    uint8_t cmd[7];
    cmd[0] = 0x02;                   
    cmd[1] = (addr >> 8) & 0xFF;
    cmd[2] = addr & 0xFF;
    cmd[3] = num.i & 0xFF;
    cmd[4] = (num.i >> 8) & 0xFF;
    cmd[5] = (num.i >> 16) & 0xFF;
    cmd[6] = (num.i >> 24) & 0xFF;

    cs_select(RAM_CS);
    spi_write_blocking(SPI_PORT, cmd, 7);
    cs_deselect(RAM_CS);
}

float spi_read_float(uint16_t addr) {
    uint8_t cmd[3];
    uint8_t read[4];

    cmd[0] = 0x03;                    
    cmd[1] = (addr >> 8) & 0xFF;
    cmd[2] = addr & 0xFF;

    cs_select(RAM_CS);
    spi_write_blocking(SPI_PORT, cmd, 3);
    spi_read_blocking(SPI_PORT, 0x00, read, 4);
    cs_deselect(RAM_CS);

    union FloatInt num;
    num.i = read[0] | (read[1] << 8) | (read[2] << 16) | (read[3] << 24);
    return num.f;
}

void writeDAC(char dac_select, float voltage) {
    uint16_t msg, voltage_bits;
    uint8_t data[2];
    char dac_select_bit;

    switch (dac_select) {
        case 'A': dac_select_bit = 0; break;
        case 'B': dac_select_bit = 1; break;
        default: printf("!!!Error in DAC Select bit\n"); return;
    }

    voltage_bits = (int)((voltage / 3.3f) * 1024.0f);
    if (voltage_bits > 1023) voltage_bits = 1023;
    if (voltage_bits < 0) voltage_bits = 0;

    msg = (dac_select_bit << 15) | (0b111 << 12) | (voltage_bits << 2);
    data[0] = msg >> 8;
    data[1] = msg & 0xFF;

    cs_select(DAC_CS);
    spi_write_blocking(SPI_PORT, data, 2);
    cs_deselect(DAC_CS);
}
