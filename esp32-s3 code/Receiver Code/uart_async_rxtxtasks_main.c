#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

static const int RX_BUF_SIZE = 1024;

// Define GPIO pins for UART0
#define TXD_PIN (GPIO_NUM_43) // Default UART0 TX
#define RXD_PIN (GPIO_NUM_44) // Default UART0 RX

// Define the UART port to use (UART0)
#define UART_PORT UART_NUM_0

// Function to initialize UART
void uart_init_receiver(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Install UART driver
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, RX_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    // Set UART pins
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // FIX: Corrected the arguments to log the correct RXD_PIN value.
    ESP_LOGI("RECEIVER_INIT", "UART Receiver configured on TX: GPIO%d, RX: GPIO%d (UART0)", TXD_PIN, RXD_PIN);
}

// RX Task: Continuously reads incoming data
static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t *data = (uint8_t *)malloc(RX_BUF_SIZE + 1); // +1 for null terminator
    while (1)
    {
        // Read bytes from UART
        const int rxBytes = uart_read_bytes(UART_PORT, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0)
        {
            data[rxBytes] = 0; // Null-terminate the received data
            ESP_LOGI(RX_TASK_TAG, "Received %d bytes: '%s'", rxBytes, data);
        }
    }
    // FIX: This code is unreachable in an infinite loop and causes a memory leak.
    // It should be removed. The task is intended to run forever.
    free(data);
    vTaskDelete(NULL);
}

// Main application entry point
void app_main(void)
{
    uart_init_receiver(); // Initialize UART for receiver
    // Create the RX task
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES - 1, NULL);
}