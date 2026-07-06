/*
 * ESP-IDF SPI Slave Receiver
 * Port of the Arduino "SPI Slave Demo" sketch (raw AVR SPCR/SPDR/SPSR
 * register slave).
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "portmacro.h"

#define SPI_HOST_USED     SPI2_HOST
#define PIN_MOSI          13
#define PIN_MISO          12
#define PIN_SCLK          14
#define PIN_CS            15
#define MAX_TRANSFER_LEN  512

static const char *TAG = "SPI_SLAVE";

static void spi_slave_init(void)
{
	spi_bus_config_t conf = {
		.miso_io_num=PIN_MISO,
		.mosi_io_num=PIN_MOSI,
		.sclk_io_num=PIN_SCLK,
		.quadhd_io_num=-1,
		.quadwp_io_num=-1
	};
	
	spi_slave_interface_config_t slave_conf = {
		.mode=0,
		.spics_io_num=PIN_CS,
		.queue_size=2,
		.flags=0
	};
	
	ESP_ERROR_CHECK(spi_slave_initialize(SPI_HOST_USED, &conf, &slave_conf, SPI_DMA_DISABLED));
}

static void spi_slave_task(void *arg)
{
    static uint8_t rx_buf[32];
	
	spi_slave_transaction_t rx={
		.length=32*8,
		.rx_buffer=rx_buf,
		.trans_len=32*8
	};

    ESP_LOGI(TAG, "Slave Initialized");

    while (1) {
		
		memset(rx_buf, 0, 32);
		
		//get actual data from nucleo
		ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &rx, portMAX_DELAY));
		rx_buf[32-1]='\0';
		ESP_LOGI(TAG, "Data received from nucleo: %s", (char*)rx_buf);
    }
}

void app_main(void)
{
    spi_slave_init();
    xTaskCreate(spi_slave_task, "spi_slave_task", 4096, NULL, 5, NULL);
}
