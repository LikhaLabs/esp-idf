/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BOTTOM 16
#define TOP    4

void app_main(void)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_reset_pin(BOTTOM);
    gpio_reset_pin(TOP);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BOTTOM, GPIO_MODE_OUTPUT);
    gpio_set_direction(TOP, GPIO_MODE_OUTPUT);

    int count = 0;

    gpio_set_level(TOP, 1);
    gpio_set_level(BOTTOM, 1);

    while(true) {
        vTaskDelay(100);
    }

    while(1) {
        /* Blink off (output low) */
        printf("Turning off the LED\n");
        gpio_set_level(BOTTOM, 0);
        if(count % 2 == 0) {
            gpio_set_level(TOP, 0);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        printf("Turning on the LED\n");
        gpio_set_level(BOTTOM, 1);
        if(count % 2 == 0) {
            gpio_set_level(TOP, 1);
        }
        vTaskDelay(20 / portTICK_PERIOD_MS);

        count++;
    }
}
