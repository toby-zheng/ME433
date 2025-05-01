/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/pwm.h"

/**
 * NOTE:
 *  Take into consideration if your WS2812 is a RGB or RGBW variant.
 *
 *  If it is RGBW, you need to set IS_RGBW to true and provide 4 bytes per 
 *  pixel (Red, Green, Blue, White) and use urgbw_u32().
 *
 *  If it is RGB, set IS_RGBW to false and provide 3 bytes per pixel (Red,
 *  Green, Blue) and use urgb_u32().
 *
 *  When RGBW is used with urgb_u32(), the White channel will be ignored (off).
 *
 */
#define IS_RGBW false
#define NUM_PIXELS 4

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 2
#endif

// Check the pin is compatible with the platform
#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

#define PWM_PIN 10
#define CYCLE_MS 5000
#define HUE_STEPS 360

// rgb struct
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

int counter;
wsColor Led1, Led2, Led3, Led4;
uint32_t Led1_32, Led2_32, Led3_32, Led4_32;
float Led1_hue, Led2_hue, Led3_hue, Led4_hue;
uint16_t wrap = 20000; // when to rollover, must be less than 65535

// todo get free sm
PIO pio;
uint sm;
uint offset;

// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (g) << 8) |
            ((uint32_t) (r) << 16) |
            (uint32_t) (b);
}

void update_hue() {
    Led1_hue = counter;
    Led2_hue = (counter + 90) % 360;
    Led3_hue = (counter + 180) % 360;
    Led4_hue = (counter + 270) % 360;
}

void color_update() {
    update_hue();
    Led1 = HSBtoRGB(Led1_hue, 1.0, 0.15);
    Led2 = HSBtoRGB(Led2_hue, 1.0, 0.15);
    Led3 = HSBtoRGB(Led3_hue, 1.0, 0.15);
    Led4 = HSBtoRGB(Led4_hue, 1.0, 0.15);

    Led1_32 = urgb_u32(Led1.r, Led1.g, Led1.b);
    Led2_32 = urgb_u32(Led2.r, Led2.g, Led2.b);
    Led3_32 = urgb_u32(Led3.r, Led3.g, Led3.b);
    Led4_32 = urgb_u32(Led4.r, Led4.g, Led4.b);

    // send pixels out
    put_pixel(pio, sm, Led1_32);
    put_pixel(pio, sm, Led2_32);
    put_pixel(pio, sm, Led3_32);
    put_pixel(pio, sm, Led4_32);

}

void PWM_init() {
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN); // Get PWM slice number
    float div = 50; // must be between 1-255
    pwm_set_clkdiv(slice_num, div); // divider
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM
    pwm_set_gpio_level(PWM_PIN, 0);
}

int main() {
    //set_sys_clock_48();

    sleep_ms(50);

    PWM_init();


    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    
    put_pixel(pio, sm, urgb_u32(255,0,0));
    sleep_ms(1000);
    // test servo to centre:
    // pwm_set_gpio_level(PWM_PIN, (7500)/2);
    // sleep_ms(1000);

    while (true) {
        for (counter = 0; counter < HUE_STEPS; counter++) {
            color_update();

            float fraction;
            if (counter <= 180) {
                fraction = ((float) counter/180.0) * 0.1 + 0.025;
            } else {
                fraction = -((float)(counter % 180)/180.0f) * 0.1 + 0.125;
            }
            

            uint16_t new_PWM = fraction * 60000;
            pwm_set_gpio_level(PWM_PIN, new_PWM);

            sleep_ms(CYCLE_MS/HUE_STEPS);
        }
    }

    

    // This will free resources and unload our program
    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
}
