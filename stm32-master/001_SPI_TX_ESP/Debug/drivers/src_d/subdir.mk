################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/src_d/stm32f44xx_USART_driver.c \
../drivers/src_d/stm32f44xx_gpio_driver.c \
../drivers/src_d/stm32f44xx_i2c_driver.c \
../drivers/src_d/stm32f44xx_rcc_driver.c \
../drivers/src_d/stm32f44xx_spi_driver.c 

OBJS += \
./drivers/src_d/stm32f44xx_USART_driver.o \
./drivers/src_d/stm32f44xx_gpio_driver.o \
./drivers/src_d/stm32f44xx_i2c_driver.o \
./drivers/src_d/stm32f44xx_rcc_driver.o \
./drivers/src_d/stm32f44xx_spi_driver.o 

C_DEPS += \
./drivers/src_d/stm32f44xx_USART_driver.d \
./drivers/src_d/stm32f44xx_gpio_driver.d \
./drivers/src_d/stm32f44xx_i2c_driver.d \
./drivers/src_d/stm32f44xx_rcc_driver.d \
./drivers/src_d/stm32f44xx_spi_driver.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/src_d/%.o drivers/src_d/%.su drivers/src_d/%.cyclo: ../drivers/src_d/%.c drivers/src_d/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F4 -DSTM32F446RETx -DNUCLEO_F446RE -c -I../Inc -I"/home/nachiket/MCU1_Testing/001_SPI_TX_ESP/drivers/inc_d" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-drivers-2f-src_d

clean-drivers-2f-src_d:
	-$(RM) ./drivers/src_d/stm32f44xx_USART_driver.cyclo ./drivers/src_d/stm32f44xx_USART_driver.d ./drivers/src_d/stm32f44xx_USART_driver.o ./drivers/src_d/stm32f44xx_USART_driver.su ./drivers/src_d/stm32f44xx_gpio_driver.cyclo ./drivers/src_d/stm32f44xx_gpio_driver.d ./drivers/src_d/stm32f44xx_gpio_driver.o ./drivers/src_d/stm32f44xx_gpio_driver.su ./drivers/src_d/stm32f44xx_i2c_driver.cyclo ./drivers/src_d/stm32f44xx_i2c_driver.d ./drivers/src_d/stm32f44xx_i2c_driver.o ./drivers/src_d/stm32f44xx_i2c_driver.su ./drivers/src_d/stm32f44xx_rcc_driver.cyclo ./drivers/src_d/stm32f44xx_rcc_driver.d ./drivers/src_d/stm32f44xx_rcc_driver.o ./drivers/src_d/stm32f44xx_rcc_driver.su ./drivers/src_d/stm32f44xx_spi_driver.cyclo ./drivers/src_d/stm32f44xx_spi_driver.d ./drivers/src_d/stm32f44xx_spi_driver.o ./drivers/src_d/stm32f44xx_spi_driver.su

.PHONY: clean-drivers-2f-src_d

