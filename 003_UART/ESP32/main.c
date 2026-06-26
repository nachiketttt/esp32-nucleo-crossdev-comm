/*
 * ESP-IDF UART Receiver (echo)
 * Port of the Arduino "UART Receiver" sketch.
 *
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "hal/uart_types.h"

#define UART_PORT      UART_NUM_2
#define UART_TX_PIN    17
#define UART_RX_PIN    16
#define UART_BAUD      115200
#define BUF_SIZE       256
#define LED_PIN        GPIO_NUM_2 // check your specific board for the right onboard LED GPIO

static const char *TAG = "UART_ECHO";

static void uart_init(void)
{
	uart_config_t conf = {
		.baud_rate=UART_BAUD,
		.data_bits=UART_DATA_8_BITS,
		.stop_bits=UART_STOP_BITS_1,
		.flow_ctrl=UART_HW_FLOWCTRL_DISABLE,
		.parity=UART_PARITY_DISABLE,
		.source_clk=UART_SCLK_DEFAULT
		};
	ESP_ERROR_CHECK(uart_param_config(UART_PORT, &conf));
	
	ESP_ERROR_CHECK(uart_set_pin(UART_PORT, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE, 0, 0, NULL, 0));
}


static void led_gpio_init(void)
{
	gpio_config_t conf={
		.intr_type=GPIO_INTR_DISABLE,
		.mode=GPIO_MODE_OUTPUT,
		.pin_bit_mask=(1ULL<<LED_PIN),
		.pull_up_en=GPIO_PULLUP_ENABLE
	};
	
	gpio_config(&conf);
}


void app_main(void)
{
    uart_init();
    led_gpio_init();

    uint8_t data[BUF_SIZE];

    while (1) 
	{
		gpio_set_level(LED_PIN, 0);
		
		uint32_t len=uart_read_bytes(UART_NUM_2, data, BUF_SIZE, pdMS_TO_TICKS(1000));
		if (len>0)
		{
			gpio_set_level(LED_PIN, 1);
			int written = uart_write_bytes(UART_PORT, (const char *)data, len);
			ESP_LOGI(TAG, "Wrote %d of %d bytes back", written, len);

			data[len] = '\0';
			ESP_LOGI(TAG, "Data: %s", (char *)data);			
		}
    }
}
