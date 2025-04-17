#include <stdio.h>
#include "pico/stdlib.h"

#define CYCLE_NS 6.667

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) { ; }
    sleep_ms(10);

    volatile float f1, f2;
    uint64_t elapsed;
    float cycles;

    printf("Enter two floats to use:\n");
    scanf("%f %f", &f1, &f2);

    volatile float f_add, f_sub, f_mult, f_div;

    absolute_time_t start = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_add = f1 + f2;
    }
    absolute_time_t end = get_absolute_time();
    elapsed = to_us_since_boot(end - start);
    cycles = ((float)elapsed * 1000.0) / CYCLE_NS;
    printf("Time for 1000 additions: %llu us\n", elapsed);
    printf("Estimated cycles per addition: %.2f\n\n", cycles / 1000.0);

    absolute_time_t start1 = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_sub = f1 - f2;
    }
    absolute_time_t end1 = get_absolute_time();
    elapsed = to_us_since_boot(end1 - start1);
    cycles = ((float)elapsed * 1000.0) / CYCLE_NS;
    printf("Time for 1000 subtractions: %llu us\n", elapsed);
    printf("Estimated cycles per subtraction: %.2f\n\n", cycles / 1000.0);

    absolute_time_t start2 = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_mult = f1 * f2;
    }
    absolute_time_t end2 = get_absolute_time();
    elapsed = to_us_since_boot(end2 - start2);
    cycles = ((float)elapsed * 1000.0) / CYCLE_NS;
    printf("Time for 1000 multiplications: %llu us\n", elapsed);
    printf("Estimated cycles per multiplication: %.2f\n\n", cycles / 1000.0);

    absolute_time_t start3 = get_absolute_time();
    for (int i = 0; i < 1000; i++) {
        f_div = f1 / f2;
    }
    absolute_time_t end3 = get_absolute_time();
    elapsed = to_us_since_boot(end3 - start3);
    cycles = ((float)elapsed * 1000.0) / CYCLE_NS;
    printf("Time for 1000 divisions: %llu us\n", elapsed);
    printf("Estimated cycles per division: %.2f\n", cycles / 1000.0);

    while (true) {
        sleep_ms(1000);
    }
}
