/*
 * main.c
 *
 * ESP-IDF SPI Slave Notify-then-Transmit Pattern INTERRUPT-BASED UART
 */

#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"

#define SPI_HOST_USED       SPI2_HOST
#define PIN_MOSI            13
#define PIN_MISO            12
#define PIN_SCLK            14
#define PIN_CS              15
#define PIN_NOTIFY          4     
#define MAX_LEN             500

#define CONSOLE_UART_NUM    UART_NUM_0
#define UART_RX_BUF_SIZE    1024

static const char *TAG = "SPI_SLAVE_NOTIFY";

static void spi_slave_init(void)
{
    spi_bus_config_t conf = {
        .miso_io_num = PIN_MISO,
        .mosi_io_num = PIN_MOSI,
        .sclk_io_num = PIN_SCLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1
    };

    spi_slave_interface_config_t slave_conf = {
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 2,
        .flags = 0
    };

    ESP_ERROR_CHECK(spi_slave_initialize(SPI_HOST_USED, &conf, &slave_conf, SPI_DMA_DISABLED));
}

static void notify_pin_init(void)
{
    gpio_config_t conf = {
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1ULL << PIN_NOTIFY),
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };

    ESP_ERROR_CHECK(gpio_config(&conf));
    gpio_set_level(PIN_NOTIFY, 0);   
}

static void uart_console_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 111200, // Matches your standard terminal speed
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    
    // Configure UART parameters on UART0
    ESP_ERROR_CHECK(uart_param_config(CONSOLE_UART_NUM, &uart_config));
    
    // Install the driver with an interrupt-driven RX ring buffer
    ESP_ERROR_CHECK(uart_driver_install(CONSOLE_UART_NUM, UART_RX_BUF_SIZE * 2, 0, 0, NULL, 0));
}

static void notify_controller(void)
{
    gpio_set_level(PIN_NOTIFY, 1);
    esp_rom_delay_us(50);
    gpio_set_level(PIN_NOTIFY, 0);
}

static uint32_t read_user_line(uint8_t *buffer)
{
    uint32_t cnt = 0;

    while (cnt < MAX_LEN - 1) {
        uint8_t ch;
        // This blocks cleanly via FreeRTOS event queues driven by hardware interrupts
        int len = uart_read_bytes(CONSOLE_UART_NUM, &ch, 1, portMAX_DELAY);
        
        if (len > 0) {
            // Echo character back to the console so you can see what you type
            printf("%c", ch);
            fflush(stdout);

            if (ch == '\r' || ch == '\n') {
                printf("\n");
                break;
            }

            buffer[cnt++] = ch;
        }
    }

    buffer[cnt] = '\0';
    return cnt;
}

static void spi_slave_task(void *arg)
{
    static uint8_t user_buffer[MAX_LEN];
    static uint8_t rx_dummy[MAX_LEN];

    ESP_LOGI(TAG, "Slave Initialized");

    while (1) {
        printf("Type anything and press Enter...\n");
        fflush(stdout);

        uint32_t len = read_user_line(user_buffer);
        ESP_LOGI(TAG, "Your message: %s", (char *)user_buffer);

        spi_slave_transaction_t msg_trans = {
            .length = (len + 1) * 8,
            .trans_len = (len + 1) * 8,
            .tx_buffer = user_buffer,
            .rx_buffer = rx_dummy,
        };

        // 1. ARM THE HARDWARE FIRST (Non-blocking)
        // The hardware is now 100% ready and waiting for master clocks.
        ESP_ERROR_CHECK(spi_slave_queue_trans(SPI_HOST_USED, &msg_trans, portMAX_DELAY));

        // 2. NOTIFY THE NUCLEO
        // Safe to tell the Nucleo to start clocking now.
        notify_controller();

        // 3. WAIT FOR COMPLETION (Blocking)
        // This blocks cleanly until the Nucleo completes the transfer.
        spi_slave_transaction_t *ret_trans;
        ESP_ERROR_CHECK(spi_slave_get_trans_result(SPI_HOST_USED, &ret_trans, portMAX_DELAY));

        ESP_LOGI(TAG, "Message sent...");
    }
}

void app_main(void)
{ 
    uart_console_init();
    spi_slave_init();
    notify_pin_init();
    xTaskCreate(spi_slave_task, "spi_slave_task", 4096, NULL, 5, NULL);
}