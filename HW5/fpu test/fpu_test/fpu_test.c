#include <stdio.h>
#include "pico/stdlib.h"


int main()

{
    stdio_init_all();

    volatile float f1, f2;
    absolute_time_t start, end;
    uint64_t elapsed;

    printf("Enter two floats to use:");
    scanf("%f %f", &f1, &f2);
    volatile float f_add, f_sub, f_mult, f_div;

    start = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_add = f1+f2;
    }
    end = get_absolute_time();
    elapsed = to_us_since_boot(end) - to_us_since_boot(start);
    printf("Addition avg time = %.3f ns\n", (elapsed * 1000.0) / 1000);

    start = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_add = f1-f2;
    }
    end = get_absolute_time();
    elapsed = to_us_since_boot(end) - to_us_since_boot(start);
    printf("Subtraction avg time = %.3f ns\n", (elapsed * 1000.0) / 1000);

    start = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_add = f1*f2;
    }
    end = get_absolute_time();
    elapsed = to_us_since_boot(end) - to_us_since_boot(start);
    printf("Multiplication avg time = %.3f ns\n", (elapsed * 1000.0) / 1000);
    
    start = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_add = f1/f2;
    }
    end = get_absolute_time();
    elapsed = to_us_since_boot(end) - to_us_since_boot(start);
    printf("Division avg time = %.3f ns\n", (elapsed * 1000.0) / 1000);
    
    
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
