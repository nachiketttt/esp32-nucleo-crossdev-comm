/*
 * esp_slave_send.c
 *
 *  Created on: 22-Jun-2026
 *      Author: nachiket
 */

 /*
  * ESP-IDF I2C Slave Transmitter + Receiver
  * Port of the Arduino "Wire Slave Transmitter and Receiver" sketch.
  */

 #include <stdint.h>
#include <stdio.h>
 #include <string.h>
#include "driver/gpio.h"
 #include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
 #include "freertos/task.h"
 #include "driver/i2c.h"
 #include "esp_log.h"
#include "hal/gpio_types.h"

 #define I2C_SLAVE_NUM      I2C_NUM_0
 #define I2C_SLAVE_SDA_IO   21
 #define I2C_SLAVE_SCL_IO   22
 #define SLAVE_ADDR         0x68
 #define RX_BUF_LEN         128
 #define TX_BUF_LEN         128

 #define CMD_GET_LEN    0x51
 #define CMD_GET_NAME   0x52

 static const char *TAG = "I2C_SLAVE";

 //static uint8_t active_command = 0xFF;
 static char name_msg[32] = "Welcome to FastBit EBA\n";

 static void i2c_slave_init(void)
 {
	 i2c_config_t conf ={
		.mode = I2C_MODE_SLAVE,
		.scl_io_num = I2C_SLAVE_SCL_IO,
		.sda_io_num = I2C_SLAVE_SDA_IO,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.slave.addr_10bit_en = 0,
		.slave.slave_addr = SLAVE_ADDR
	};
	
	ESP_ERROR_CHECK(i2c_param_config(I2C_SLAVE_NUM, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_SLAVE_NUM, conf.mode,
	                                    RX_BUF_LEN, TX_BUF_LEN, 0));
 }

 static uint8_t get_len_of_data(void)
 {
	uint8_t len;
	len=strlen(name_msg);
    return len;
 }

 static void i2c_slave_task(void *arg)
 {
	uint8_t rx_buffer[64]={0};
     ESP_LOGI(TAG, "Slave is ready : Address 0x%x", SLAVE_ADDR);

     while (1) 
	 {
		 // Blocks (up to the timeout) until the master writes something
		 int len = i2c_slave_read_buffer(I2C_SLAVE_NUM, rx_buffer,
		                                  sizeof(rx_buffer) - 1,
		                                  pdMS_TO_TICKS(1000));
		 if (len > 0) 
		 {
		     rx_buffer[len] = '\0';
		     ESP_LOGI(TAG, "Received: %x", rx_buffer[0]);
		 }
		 
		 if (*rx_buffer==0x51) 
		 {
			i2c_reset_tx_fifo(I2C_SLAVE_NUM);
			uint8_t len_info = get_len_of_data();
			i2c_slave_write_buffer(I2C_SLAVE_NUM, &len_info, 1, pdMS_TO_TICKS(1000));
			*rx_buffer=0;
			ESP_LOGI(TAG, "Sent data length to nucleo : %d \n",len_info);
		 }
		 
		 if (*rx_buffer==0x52) 
		 {
			i2c_reset_tx_fifo(I2C_SLAVE_NUM);
		 	i2c_slave_write_buffer(I2C_SLAVE_NUM, (uint8_t*)name_msg, strlen(name_msg), pdMS_TO_TICKS(1000));
			*rx_buffer=0;
			ESP_LOGI(TAG, "Sent data to nucleo\n");
		 }
	 }
 }

 void app_main(void)
 {
     i2c_slave_init();
     xTaskCreate(i2c_slave_task, "i2c_slave_task", 4096, NULL, 5, NULL);
 }
 
 
 
 
