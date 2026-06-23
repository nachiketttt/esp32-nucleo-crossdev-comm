/*
 * ESP-IDF I2C Slave Receiver
 * Ported from the Arduino "Wire Slave Receiver" sketch.
 *
 *
 * Wiring:
 *   GPIO21 -> SDA (to STM32 master's I2C SDA, e.g. PB7)
 *   GPIO22 -> SCL (to STM32 master's I2C SCL, e.g. PB6)
 *   GND    -> common ground
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define I2C_SLAVE_NUM      I2C_NUM_0
#define I2C_SLAVE_SDA_IO   21
#define I2C_SLAVE_SCL_IO   22
#define MY_ADDR            0x68
#define RX_BUF_LEN         128
#define TX_BUF_LEN         128

static const char *TAG = "I2C_SLAVE";

static void i2c_slave_init(void)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_SLAVE,
        .sda_io_num = I2C_SLAVE_SDA_IO,
        .scl_io_num = I2C_SLAVE_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .slave.addr_10bit_en = 0,
        .slave.slave_addr = MY_ADDR,
    };

    ESP_ERROR_CHECK(i2c_param_config(I2C_SLAVE_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_SLAVE_NUM, conf.mode,
                                        RX_BUF_LEN, TX_BUF_LEN, 0));
}

// rather than interrupt/callback-based, this runs as its own task.
static void i2c_slave_task(void *arg)
{
    uint8_t rx_buffer[64];

    ESP_LOGI(TAG, "Slave is ready : Address 0x%x", MY_ADDR);
    ESP_LOGI(TAG, "Waiting for data from master");

    while (1) {
        // Blocks (up to the timeout) until the master writes something
        int len = i2c_slave_read_buffer(I2C_SLAVE_NUM, rx_buffer,
                                         sizeof(rx_buffer) - 1,
                                         pdMS_TO_TICKS(1000));
        if (len > 0) {
            rx_buffer[len] = '\0';
            ESP_LOGI(TAG, "Received: %s", (char *)rx_buffer);
        }
    }
}

void app_main(void)
{
    i2c_slave_init();
    xTaskCreate(i2c_slave_task, "i2c_slave_task", 4096, NULL, 5, NULL);
}
