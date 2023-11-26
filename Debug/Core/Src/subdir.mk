################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/es_wifi.c \
../Core/Src/es_wifi_io.c \
../Core/Src/freertos.c \
../Core/Src/html_builder.c \
../Core/Src/main.c \
../Core/Src/stm32l4s5i_iot01.c \
../Core/Src/stm32l4s5i_iot01_accelero.c \
../Core/Src/stm32l4s5i_iot01_gyro.c \
../Core/Src/stm32l4s5i_iot01_hsensor.c \
../Core/Src/stm32l4s5i_iot01_magneto.c \
../Core/Src/stm32l4s5i_iot01_psensor.c \
../Core/Src/stm32l4s5i_iot01_tsensor.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/wifi.c 

OBJS += \
./Core/Src/es_wifi.o \
./Core/Src/es_wifi_io.o \
./Core/Src/freertos.o \
./Core/Src/html_builder.o \
./Core/Src/main.o \
./Core/Src/stm32l4s5i_iot01.o \
./Core/Src/stm32l4s5i_iot01_accelero.o \
./Core/Src/stm32l4s5i_iot01_gyro.o \
./Core/Src/stm32l4s5i_iot01_hsensor.o \
./Core/Src/stm32l4s5i_iot01_magneto.o \
./Core/Src/stm32l4s5i_iot01_psensor.o \
./Core/Src/stm32l4s5i_iot01_tsensor.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/wifi.o 

C_DEPS += \
./Core/Src/es_wifi.d \
./Core/Src/es_wifi_io.d \
./Core/Src/freertos.d \
./Core/Src/html_builder.d \
./Core/Src/main.d \
./Core/Src/stm32l4s5i_iot01.d \
./Core/Src/stm32l4s5i_iot01_accelero.d \
./Core/Src/stm32l4s5i_iot01_gyro.d \
./Core/Src/stm32l4s5i_iot01_hsensor.d \
./Core/Src/stm32l4s5i_iot01_magneto.d \
./Core/Src/stm32l4s5i_iot01_psensor.d \
./Core/Src/stm32l4s5i_iot01_tsensor.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/wifi.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/Philippe/STM32CubeIDE/workspace_1.13.1/Smart_Home/Middlewares" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/es_wifi.cyclo ./Core/Src/es_wifi.d ./Core/Src/es_wifi.o ./Core/Src/es_wifi.su ./Core/Src/es_wifi_io.cyclo ./Core/Src/es_wifi_io.d ./Core/Src/es_wifi_io.o ./Core/Src/es_wifi_io.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/html_builder.cyclo ./Core/Src/html_builder.d ./Core/Src/html_builder.o ./Core/Src/html_builder.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32l4s5i_iot01.cyclo ./Core/Src/stm32l4s5i_iot01.d ./Core/Src/stm32l4s5i_iot01.o ./Core/Src/stm32l4s5i_iot01.su ./Core/Src/stm32l4s5i_iot01_accelero.cyclo ./Core/Src/stm32l4s5i_iot01_accelero.d ./Core/Src/stm32l4s5i_iot01_accelero.o ./Core/Src/stm32l4s5i_iot01_accelero.su ./Core/Src/stm32l4s5i_iot01_gyro.cyclo ./Core/Src/stm32l4s5i_iot01_gyro.d ./Core/Src/stm32l4s5i_iot01_gyro.o ./Core/Src/stm32l4s5i_iot01_gyro.su ./Core/Src/stm32l4s5i_iot01_hsensor.cyclo ./Core/Src/stm32l4s5i_iot01_hsensor.d ./Core/Src/stm32l4s5i_iot01_hsensor.o ./Core/Src/stm32l4s5i_iot01_hsensor.su ./Core/Src/stm32l4s5i_iot01_magneto.cyclo ./Core/Src/stm32l4s5i_iot01_magneto.d ./Core/Src/stm32l4s5i_iot01_magneto.o ./Core/Src/stm32l4s5i_iot01_magneto.su ./Core/Src/stm32l4s5i_iot01_psensor.cyclo ./Core/Src/stm32l4s5i_iot01_psensor.d ./Core/Src/stm32l4s5i_iot01_psensor.o ./Core/Src/stm32l4s5i_iot01_psensor.su ./Core/Src/stm32l4s5i_iot01_tsensor.cyclo ./Core/Src/stm32l4s5i_iot01_tsensor.d ./Core/Src/stm32l4s5i_iot01_tsensor.o ./Core/Src/stm32l4s5i_iot01_tsensor.su ./Core/Src/stm32l4xx_it.cyclo ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/stm32l4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32l4xx.cyclo ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/system_stm32l4xx.su ./Core/Src/wifi.cyclo ./Core/Src/wifi.d ./Core/Src/wifi.o ./Core/Src/wifi.su

.PHONY: clean-Core-2f-Src

