#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include <math.h>
#include <stdlib.h>

// PIN definitions
// LEFT
#define IN1 17
#define IN2 16 
// RIGHT
#define IN3 18
#define IN4 19

// function declarations
void init_pwm_pin(int pin, float clkdiv, uint16_t wrap);
void set_left_motor(float duty_cycle);
void set_right_motor(float duty_cycle);


int main()
{
    stdio_init_all();

    // Initialize PWM pins
    init_pwm_pin(IN1, 1.0f, 6250); // 6250 is the wrap value for 20kHz PWM
    init_pwm_pin(IN2, 1.0f, 6250);
    init_pwm_pin(IN3, 1.0f, 6250);
    init_pwm_pin(IN4, 1.0f, 6250);

    // init to 0% duty cycle
    int duty = 0;
    set_left_motor((float)duty);
    set_right_motor((float)duty);

    while (true) {
        char input;
        printf("Current Duty Cycle: %d%%\n Enter '+' or '-' to increase or decrease, '0' to stop: ", duty);

        scanf(" %c", &input);
        if (input == '+') {
            duty += 1; 
            if (duty > 100) {
                duty = 100; // Cap at 100%
                printf("Duty cycle capped at 100%%\n");
            }
        } else if (input == '-') {
            duty -= 1; 
            if (duty < -100) {
                duty = -100; // Cap at -100%
                printf("Duty cycle capped at -100%%\n");
            }
        } else if (input == '0') {
            duty = 0; // Stop motors
        } else {
            printf("Invalid input. Please enter '+' or '-' or '0'.\n");
        }
        // Set motors with the current duty cycle
        set_left_motor((float)duty);
        set_right_motor((float)duty);
        printf("New Duty Cycle: %d%%\n", duty);

        sleep_ms(50);
    }
}

void init_pwm_pin(int pin, float clkdiv, uint16_t wrap) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_clkdiv(slice_num, clkdiv);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);   

    pwm_set_gpio_level(pin, 0); // Initialize PWM level to 0
}

void set_left_motor(float duty_cycle) {
    if (duty_cycle < -100.0 || duty_cycle > 100.0) {
        printf("RIGHT: Duty cycle must be between -100.0 and 100.0\n");
        return;
    }
    if (duty_cycle < 0.0f) {
        // Reverse direction
        duty_cycle = -duty_cycle; // Make it positive for PWM control
        uint16_t level = (uint16_t)((duty_cycle * 6250)/100.0); 
        pwm_set_gpio_level(IN1, level);
        pwm_set_gpio_level(IN2, 0); 
    }
    else if (duty_cycle > 0.0f) {
        // Forward direction
        uint16_t level = (uint16_t)((duty_cycle * 6250)/100.0); 
        pwm_set_gpio_level(IN2, level);
        pwm_set_gpio_level(IN1, 0); 
    }
    else {
        // Stop
        pwm_set_gpio_level(IN1, 0);
        pwm_set_gpio_level(IN2, 0);
    }
    return;
}

void set_right_motor(float duty_cycle) {
    if (duty_cycle < -100.0 || duty_cycle > 100.0) {
        printf("RIGHT: Duty cycle must be between -100.0 and 100.0\n");
        return;
    }   
    if (duty_cycle < 0.0f) {
        // Reverse direction
        duty_cycle = -duty_cycle; // Make it positive for PWM control
        uint16_t level = (uint16_t)((duty_cycle * 6250)/100.0); 
        pwm_set_gpio_level(IN3, level);
        pwm_set_gpio_level(IN4, 0); 
    }
    else if (duty_cycle > 0.0f) {
        // Forward direction
        uint16_t level = (uint16_t)((duty_cycle * 6250)/100.0); 
        pwm_set_gpio_level(IN4, level);
        pwm_set_gpio_level(IN3, 0); 
    }
    else {
        // Stop
        pwm_set_gpio_level(IN3, 0);
        pwm_set_gpio_level(IN4, 0);
    }
    return;
}