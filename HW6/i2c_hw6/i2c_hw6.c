#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#define DEVICE_ADDRESS 0b0100000
#define IODIR_ADDRESS 0x00
#define IOCON_ADDRESS 0x05
#define GPIO_ADDRESS 0x09
#define OLAT_ADDRESS 0x0A

// read and write buffer
static uint8_t buf;
uint8_t I2C[2];

// functions to toggle onboard LED
static int button_voltage = 1;     // init to 1 because button pulled high
static int led_voltage = 0;        // init led off
void default_led_init(void);
void default_led_toggle(void);

// functions to read/write to GPIO pins for I2C
void gpio_expander_init(void);
uint8_t gpio_exp_readPin(uint8_t device, uint8_t register);
void gpio_exp_writePin(uint8_t device, uint8_t register, uint8_t data);



int main()
{
    stdio_init_all();
    default_led_init();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    gpio_expander_init();

    
    while (true) {
        buf = gpio_exp_readPin(DEVICE_ADDRESS, GPIO_ADDRESS);
         
        // check button
        if ((buf & 1) != button_voltage) {
            led_voltage = !led_voltage;
            gpio_exp_writePin(DEVICE_ADDRESS, OLAT_ADDRESS, ((led_voltage) << 7));
            button_voltage = (buf & 1);
        }

        //heartbeat leddfasd
        default_led_toggle();
        sleep_ms(150);
    }
}


void default_led_init() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void default_led_toggle() {
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
}

void gpio_expander_init() {
    // enable byte mode
    gpio_exp_writePin(DEVICE_ADDRESS, IOCON_ADDRESS, 0b00100000);

    // GPIO7 output, rest stay input
    gpio_exp_writePin(DEVICE_ADDRESS, IODIR_ADDRESS, 0b01111111);

    // default voltage of GPIO7 as low
    gpio_exp_writePin(DEVICE_ADDRESS, OLAT_ADDRESS, 0b00000000);
}

uint8_t gpio_exp_readPin(uint8_t device, uint8_t reg_addr) {
    uint8_t data;
    if (i2c_write_blocking(I2C_PORT, device, &reg_addr, 1, true) < 0) {
        printf("I2C write failed\n");
    }

    if (i2c_read_blocking(I2C_PORT, device, &data, 1, false) < 0) {
        printf("I2C read failed\n");
    }

    return data;
}

void gpio_exp_writePin(uint8_t device, uint8_t reg_addr, uint8_t data) {
    uint8_t msg[2];
    msg[0] = reg_addr;
    msg[1] = data;

    int result = i2c_write_blocking(I2C_PORT, device, msg, 2, false);
    if (result < 0) {
        printf("Unable to write to GPIO Extender\n");
    }
}