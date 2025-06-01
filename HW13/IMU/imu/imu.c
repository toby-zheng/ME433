#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

#define IMU_ADDR       0x68
#define INT_ENABLE     0x38

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

#define I2C_PORT       i2c0
#define I2C_SDA_PIN    8
#define I2C_SCL_PIN    9
#define I2C_BAUDRATE   100000

#define OLED_WIDTH     128
#define OLED_HEIGHT    32

#define CENTER_X  (OLED_WIDTH  / 2)
#define CENTER_Y  (OLED_HEIGHT / 2)
#define PIXELS_PER_G  30.0f

static int16_t accel_raw_x, accel_raw_y, accel_raw_z;
static int16_t temp_raw;
static int16_t gyro_raw_x, gyro_raw_y, gyro_raw_z;
static uint8_t imu_buf[14];

//function declarations
void default_led_init(void);
void default_led_toggle(void);
void imu_init(void);
int  imu_check(void);
bool imu_read_all(void);
void draw_line(int x0, int y0, int x1, int y1, uint8_t color);


int main() {
    stdio_init_all();
    default_led_init();

    i2c_init(I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_setup();
    ssd1306_clear();
    ssd1306_update();

    imu_init();
    sleep_ms(50);
    if (imu_check() != 1) {
        while (true) {
            default_led_toggle();
            sleep_ms(100);
        }
    }

    while (true) {
        default_led_toggle();

        if (!imu_read_all()) {
            sleep_ms(10);
            continue;
        }

        float ax_g = (float)accel_raw_x / 16384.0f;     
        float ay_g = (float)accel_raw_y / 16384.0f;

        // negative x-axis is right, positive y-axis is down
        float dx = -ax_g * PIXELS_PER_G;
        float dy = ay_g * PIXELS_PER_G;

        int x_end = CENTER_X + (int)roundf(dx);
        int y_end = CENTER_Y + (int)roundf(dy);

        ssd1306_clear();
        draw_line(CENTER_X, CENTER_Y, x_end, y_end, 1);
        ssd1306_update();

        // 100 Hz
        sleep_ms(10);
    }

    return 0;
}

void default_led_init() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void default_led_toggle() {
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
}

static bool imu_write_reg(uint8_t reg_addr, uint8_t data) {
    uint8_t buf[2] = { reg_addr, data };
    int ret = i2c_write_blocking(I2C_PORT, IMU_ADDR, buf, 2, false);
    return (ret == 2);
}

static bool imu_read_regs(uint8_t start_reg, uint8_t *dst, size_t len) {
    int w = i2c_write_blocking(I2C_PORT, IMU_ADDR, &start_reg, 1, true);
    if (w != 1) return false;
    int r = i2c_read_blocking(I2C_PORT, IMU_ADDR, dst, len, false);
    return (r == (int)len);
}

void imu_init() {
    uint8_t cmd[2];

    cmd[0] = PWR_MGMT_1;
    cmd[1] = 0x80;
    i2c_write_blocking(I2C_PORT, IMU_ADDR, cmd, 2, false);
    sleep_ms(100);

    cmd[0] = PWR_MGMT_1;
    cmd[1] = 0x00;
    i2c_write_blocking(I2C_PORT, IMU_ADDR, cmd, 2, false);
    sleep_ms(10);

    cmd[0] = GYRO_CONFIG;
    cmd[1] = 0x18;
    i2c_write_blocking(I2C_PORT, IMU_ADDR, cmd, 2, false);
    sleep_ms(10);

    cmd[0] = ACCEL_CONFIG;
    cmd[1] = 0x00;
    i2c_write_blocking(I2C_PORT, IMU_ADDR, cmd, 2, false);
    sleep_ms(10);

    cmd[0] = INT_ENABLE;
    cmd[1] = 0x01;
    i2c_write_blocking(I2C_PORT, IMU_ADDR, cmd, 2, false);
    sleep_ms(10);
}


int imu_check() {
    uint8_t reg = WHO_AM_I;
    uint8_t resp;
    int w = i2c_write_blocking(I2C_PORT, IMU_ADDR, &reg, 1, true);
    if (w != 1) return 2;
    int r = i2c_read_blocking(I2C_PORT, IMU_ADDR, &resp, 1, false);
    if (r != 1) return 2;
  if(resp == 0x68) {
    return 1;
   } else {
    return 0;
   }
}

// read data from IMU
bool imu_read_all() {
    if (!imu_read_regs(ACCEL_XOUT_H, imu_buf, 14)) {
        return false;
    }
    accel_raw_x = (int16_t)((imu_buf[0] << 8)  | imu_buf[1]);
    accel_raw_y = (int16_t)((imu_buf[2] << 8)  | imu_buf[3]);
    accel_raw_z = (int16_t)((imu_buf[4] << 8)  | imu_buf[5]);
    temp_raw    = (int16_t)((imu_buf[6] << 8)  | imu_buf[7]);
    gyro_raw_x  = (int16_t)((imu_buf[8] << 8)  | imu_buf[9]);
    gyro_raw_y  = (int16_t)((imu_buf[10] << 8) | imu_buf[11]);
    gyro_raw_z  = (int16_t)((imu_buf[12] << 8) | imu_buf[13]);
    return true;
}

// Bresenham line algorithm
void draw_line(int x0, int y0, int x1, int y1, uint8_t color) {
    int dx = abs(x1 - x0);
    int sx, sy;
    if (x0 < x1) {
        sx = 1;
    } else {
        sx = -1;
    }
    int dy = -abs(y1 - y0);
    if (y0 < y1) {
        sy = 1;
    } else {
        sy = -1;
    }
    int err = dx + dy;

    while (true) {
        if (x0 >= 0 && x0 < OLED_WIDTH && y0 >= 0 && y0 < OLED_HEIGHT) {
            ssd1306_drawPixel((uint8_t)x0, (uint8_t)y0, color);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0   += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0   += sy;
        }
    }
}
