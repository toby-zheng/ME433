#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9


#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"

int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, camera!\n");

    init_camera_pins();
 
    while (true) {
        // uncomment these and printImage() when testing with python 
        char m[10];
        scanf("%s",m);

        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the ine
        setPixel(IMAGESIZEY/2,com,0,255,0); // draw the center so you can see it in python
        printImage();
        // printf("%d\r\n",com); // comment this when testing with python
    }
}



// int main()
// {
//     stdio_init_all();

//     // I2C Initialisation. Using it at 400Khz.
//     i2c_init(I2C_PORT, 400*1000);
    
//     gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
//     gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
//     gpio_pull_up(I2C_SDA);
//     gpio_pull_up(I2C_SCL);
//     // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

//     while (true) {
//         printf("Hello, world!\n");
//         sleep_ms(1000);
//     }
// }
