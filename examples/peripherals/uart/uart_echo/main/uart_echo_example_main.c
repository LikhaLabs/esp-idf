/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/**
 * This is an example which echos any data it receives on configured UART back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */

#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_BAUD_RATE     921600
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

#define BUF_SIZE (1024)

SemaphoreHandle_t printf_Mutex;

void print_hex(const uint8_t* buf, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        if (i > 0) printf(" ");
        printf("%02X", buf[i]);
    }
    printf("\n");
}

static void echo_task(void *arg)
{
    int uart_num = *((int*) arg);

    int tx =0, rx =0;

    if (uart_num == 1) {
        tx = 26;
        rx = 27;
    } else {
        tx = 16;
        rx = 17;
    }

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = ECHO_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx, rx, ECHO_TEST_RTS, ECHO_TEST_CTS));

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    printf("UART%d started.\n", uart_num);

    while (1) {
        // Read data from the UART
        memset(data, 0, BUF_SIZE);
        int len = uart_read_bytes(uart_num, data, BUF_SIZE, 20 / portTICK_RATE_MS);

        if (len > 0) {
            xSemaphoreTake(printf_Mutex, portMAX_DELAY);
            printf("UART%d: (%d bytes)\n", uart_num, len);
            print_hex(data, len);
            printf("\n");
            xSemaphoreGive(printf_Mutex);
        }
    }
}

void app_main(void)
{
    int uart1 = UART_NUM_1, uart2 = UART_NUM_2;

    printf_Mutex = xSemaphoreCreateMutex();

    xTaskCreate(echo_task, "uart_echo_task1", ECHO_TASK_STACK_SIZE, &uart1, 10, NULL);
    xTaskCreate(echo_task, "uart_echo_task2", ECHO_TASK_STACK_SIZE, &uart2, 10, NULL);

}
