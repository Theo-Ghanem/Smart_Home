################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Components/mfxstm32l152/mfxstm32l152.c 

OBJS += \
./Core/Components/mfxstm32l152/mfxstm32l152.o 

C_DEPS += \
./Core/Components/mfxstm32l152/mfxstm32l152.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Components/mfxstm32l152/%.o Core/Components/mfxstm32l152/%.su Core/Components/mfxstm32l152/%.cyclo: ../Core/Components/mfxstm32l152/%.c Core/Components/mfxstm32l152/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/Philippe/STM32CubeIDE/workspace_1.13.1/Smart_Home/Middlewares" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Components-2f-mfxstm32l152

clean-Core-2f-Components-2f-mfxstm32l152:
	-$(RM) ./Core/Components/mfxstm32l152/mfxstm32l152.cyclo ./Core/Components/mfxstm32l152/mfxstm32l152.d ./Core/Components/mfxstm32l152/mfxstm32l152.o ./Core/Components/mfxstm32l152/mfxstm32l152.su

.PHONY: clean-Core-2f-Components-2f-mfxstm32l152

