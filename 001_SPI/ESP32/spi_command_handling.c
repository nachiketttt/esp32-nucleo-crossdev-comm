/*
 * main.c
 *
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/gpio_types.h"

#define SPI_HOST_USED     SPI2_HOST
#define PIN_MOSI          13
#define PIN_MISO          12
#define PIN_SCLK          14
#define PIN_CS            15

#define NACK  0xA5
#define ACK   0xF5

#define COMMAND_LED_CTRL     0x50
#define COMMAND_SENSOR_READ  0x51
#define COMMAND_LED_READ     0x52
#define COMMAND_PRINT        0x53
#define COMMAND_ID_READ      0x54

#define LED_ON   1
#define LED_OFF  0

#define FRAME_LEN  32   // both directions; under the 64-byte no-DMA FIFO cap

static const char *TAG = "SPI_SLAVE_CMD";
static const uint8_t board_id[] = "ESP32WROOM";

static adc_oneshot_unit_handle_t adc_handle = NULL;

static uint8_t rx_cmd[FRAME_LEN];
static uint8_t tx_dummy[FRAME_LEN];
static uint8_t tx_resp[FRAME_LEN];
static uint8_t rx_dummy[FRAME_LEN];


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

static uint8_t checkData(uint8_t command)
{
    return ACK;
}

static void handle_led_ctrl(void)
{
	spi_slave_transaction_t arg_trans = {
	    .length = 2 * 8,
	    .trans_len = 2 * 8,
	    .tx_buffer = tx_dummy,
	    .rx_buffer = rx_cmd,
	};
	ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &arg_trans, portMAX_DELAY));

	uint8_t pin = rx_cmd[0];
	uint8_t value = rx_cmd[1];
	
    if (!GPIO_IS_VALID_OUTPUT_GPIO(pin)) {
        ESP_LOGE(TAG, "LED_CTRL: pin %d is not a valid output GPIO, ignoring", pin);
        return;
    }

    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_level(pin, value);
    ESP_LOGI(TAG, "LED_CTRL: pin %d -> %d", pin, value);
}

static uint8_t handle_sensor_read()
{
	spi_slave_transaction_t arg_trans = {
	    .length = 1 * 8,
	    .trans_len = 1 * 8,
	    .tx_buffer = tx_dummy,
	    .rx_buffer = rx_cmd,
	};
	ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &arg_trans, portMAX_DELAY));

	uint8_t pin = rx_cmd[0];
	
    adc_unit_t unit;
    adc_channel_t channel;

    esp_err_t err = adc_oneshot_io_to_channel(pin, &unit, &channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SENSOR_READ: GPIO %d is not ADC-capable", pin);
        return 0;
    }

    if (adc_handle == NULL) {
        adc_oneshot_unit_init_cfg_t init_cfg = { .unit_id = unit };
        ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_cfg, &adc_handle));
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, channel, &chan_cfg));

    int raw = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, channel, &raw));

    // ESP32 ADC is 12-bit (0-4095), not the AVR's 10-bit (0-1023) --
    // scale down to 0-255 accordingly.
    uint8_t val = (uint8_t)((raw * 255) / 4095);
    ESP_LOGI(TAG, "SENSOR_READ: GPIO %d raw=%d -> %d", pin, raw, val);
	
	tx_dummy[0] = val;
	spi_slave_transaction_t resp_trans = {
	        .length = 1 * 8,
			.trans_len = 1 * 8,
	        .tx_buffer = tx_dummy, 
			.rx_buffer = rx_dummy,
	};
	ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &resp_trans, portMAX_DELAY));
		
    return val;
}

static void handle_led_read(void)
{
	spi_slave_transaction_t arg_trans = {
	    .length = 1 * 8,
	    .trans_len = 1 * 8,
	    .tx_buffer = tx_dummy,
	    .rx_buffer = rx_cmd,
	};
	ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &arg_trans, portMAX_DELAY));

	uint8_t pin = rx_cmd[0];
	
    if (!GPIO_IS_VALID_GPIO(pin)) {
        ESP_LOGE(TAG, "LED_READ: pin %d is not a valid GPIO, ignoring", pin);
    }
	
	
    int val = gpio_get_level(pin);
	tx_dummy[0] = (uint8_t)val;
    ESP_LOGI(TAG, "LED_READ: pin %d = %u", pin, val);
	
	spi_slave_transaction_t led_trans = {
	    .length = 1 * 8,
	    .trans_len = 1 * 8,
	    .tx_buffer = tx_dummy,
	    .rx_buffer = rx_cmd,
	};
	ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &led_trans, portMAX_DELAY));
}

 static void handle_print(void)
 {
     spi_slave_transaction_t arg_trans = {
         .length = 1 * 8,
         .trans_len = 1 * 8,
         .tx_buffer = tx_dummy,
         .rx_buffer = rx_cmd,
     };
     ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &arg_trans, portMAX_DELAY));

     uint8_t len = rx_cmd[0];          // actually use the length the master sent
     if (len > FRAME_LEN - 1) {
         ESP_LOGE(TAG, "PRINT: length %d too big, truncating", len);
         len = FRAME_LEN - 1;
     }

     static uint8_t rx_buf[FRAME_LEN];
     spi_slave_transaction_t data_trans = {
         .length = len * 8,            // matches the master's real data-pulse size
         .trans_len = len * 8,
         .tx_buffer = tx_dummy,
         .rx_buffer = rx_buf,
     };
     ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &data_trans, portMAX_DELAY));

     rx_buf[len] = '\0';
     ESP_LOGI(TAG, "Data received from nucleo: %s", (char *)rx_buf);
 }

 static void handle_id_read(void)
 {
     //uint8_t len = (uint8_t)strlen((const char *)board_id);
     memcpy(tx_dummy, board_id, 11);   // full string, not truncated to 8

     spi_slave_transaction_t id_trans = {
         .length = 11 * 8,
         .trans_len = 11 * 8,
         .tx_buffer = tx_dummy,
         .rx_buffer = rx_dummy,
     };
     ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &id_trans, portMAX_DELAY));
     ESP_LOGI(TAG, "ID_READ: %s", board_id);
 }

static void spi_slave_task(void *arg)
{

    ESP_LOGI(TAG, "Slave Initialized");

    while (1) 
	{
        // ---- Transaction 1: receive the command frame ----
        spi_slave_transaction_t cmd_trans = {
            .length = 1 * 8,
            .trans_len = 1 * 8,
            .tx_buffer = tx_dummy,
            .rx_buffer = rx_cmd,
        };
        ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &cmd_trans, portMAX_DELAY));

        uint8_t command = rx_cmd[0];

        uint8_t ack = checkData(command);
        memset(tx_resp, 0, FRAME_LEN);
        tx_resp[0] = ack;
		
		// ---- Transaction 2: send the response frame ----
		spi_slave_transaction_t resp_trans = {
		    .length = 1 * 8,
		    .trans_len = 1 * 8,
		    .tx_buffer = tx_resp,
		    .rx_buffer = rx_dummy,
		};
		ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &resp_trans, portMAX_DELAY));
		
		/*
		spi_slave_transaction_t arg_trans = {
		    .length = 2 * 8,
		    .trans_len = 2 * 8,
		    .tx_buffer = tx_dummy,
		    .rx_buffer = rx_cmd,
		};
		ESP_ERROR_CHECK(spi_slave_transmit(SPI_HOST_USED, &arg_trans, portMAX_DELAY));
		
		uint8_t arg1 = rx_cmd[0];
		uint8_t arg2 = rx_cmd[1];
		*/

        // ---- Build the response payload based on the command ----
        switch (command) {
            case COMMAND_LED_CTRL:
                handle_led_ctrl();
                break;

            case COMMAND_SENSOR_READ:
                handle_sensor_read();
                break;

            case COMMAND_LED_READ:
                handle_led_read();
                break;
				
            case COMMAND_PRINT:
                handle_print();
                break;

            case COMMAND_ID_READ:
                handle_id_read();
                break;

            default:
                ESP_LOGE(TAG, "Unknown command 0x%x", command);
                tx_resp[0] = NACK;
                break;
        }
    }
}

void app_main(void)
{
    spi_slave_init();
    xTaskCreate(spi_slave_task, "spi_slave_task", 4096, NULL, 5, NULL);
}
