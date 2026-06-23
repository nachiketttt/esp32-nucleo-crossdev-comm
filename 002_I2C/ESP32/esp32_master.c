/*
 * main.c
 *
 *  Created on: 23-Jun-2026
 *      Author: nachiket
 */

 /*
  * ESP-IDF I2C Master Transmitter + Receiver - LEGACY driver (driver/i2c.h)
  * Port of the Arduino "Wire Master Transmitter and Receiver" sketch.
  *
  */

 #include <stdio.h>
 #include <string.h>
 #include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
 #include "freertos/task.h"
 #include "driver/i2c.h"
 #include "esp_log.h"
#include "hal/i2c_types.h"

 #define I2C_MASTER_NUM       I2C_NUM_0 
 #define I2C_MASTER_SDA_IO    21
 #define I2C_MASTER_SCL_IO    22
 #define I2C_MASTER_FREQ_HZ   100000
 #define SLAVE_ADDR           0x68

 #define CMD_GET_LEN    0x51
 #define CMD_GET_NAME   0x52

 static const char *TAG = "I2C_MASTER";

static void i2c_master_init(void)
{
	  i2c_config_t conf ={
	 	.mode = I2C_MODE_MASTER,
	 	.scl_io_num = I2C_MASTER_SCL_IO,
	 	.sda_io_num = I2C_MASTER_SDA_IO,
	 	.scl_pullup_en = GPIO_PULLUP_ENABLE,
	 	.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C_MASTER_FREQ_HZ
	 };

	 ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
	 ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
}

static void i2c_master_task(void *arg)
{
     uint8_t cmd;
     uint8_t len_info = 0;
     uint8_t rcv_buf[32] = {0};

     while (1) {
         // ---- Step 1: request the length ----
		 cmd = CMD_GET_LEN;
		 i2c_master_write_to_device(I2C_MASTER_NUM, SLAVE_ADDR, &cmd, 1, pdMS_TO_TICKS(1000));
		 
		 //read len_info from nucleo
		 i2c_master_read_from_device(I2C_MASTER_NUM, SLAVE_ADDR, &len_info, 1, pdMS_TO_TICKS(1000));
         ESP_LOGI(TAG, "Data Length: %d", len_info);

         // ---- Step 2: request the actual name string ----
		 cmd = CMD_GET_NAME;
		 i2c_master_write_to_device(I2C_MASTER_NUM, SLAVE_ADDR, &cmd, 1, pdMS_TO_TICKS(1000));

		 //read data from nucleo
		 i2c_master_read_from_device(I2C_MASTER_NUM, SLAVE_ADDR, rcv_buf, len_info, pdMS_TO_TICKS(1000));

         rcv_buf[len_info] = '\0';
         ESP_LOGI(TAG, "Data: %s", (char *)rcv_buf);
         ESP_LOGI(TAG, "*********************END*********************");

         vTaskDelay(pdMS_TO_TICKS(2000));
     }
}

void app_main(void)
{
     i2c_master_init();
     xTaskCreate(i2c_master_task, "i2c_master_task", 4096, NULL, 5, NULL);
}



