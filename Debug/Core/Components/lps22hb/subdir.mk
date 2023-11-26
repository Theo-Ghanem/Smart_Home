################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Components/lps22hb/lps22hb.c 

OBJS += \
./Core/Components/lps22hb/lps22hb.o 

C_DEPS += \
./Core/Components/lps22hb/lps22hb.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Components/lps22hb/%.o Core/Components/lps22hb/%.su Core/Components/lps22hb/%.cyclo: ../Core/Components/lps22hb/%.c Core/Components/lps22hb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/Philippe/STM32CubeIDE/workspace_1.13.1/Smart_Home/Middlewares" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Components-2f-lps22hb

clean-Core-2f-Components-2f-lps22hb:
	-$(RM) ./Core/Components/lps22hb/lps22hb.cyclo ./Core/Components/lps22hb/lps22hb.d ./Core/Components/lps22hb/lps22hb.o ./Core/Components/lps22hb/lps22hb.su

.PHONY: clean-Core-2f-Components-2f-lps22hb

