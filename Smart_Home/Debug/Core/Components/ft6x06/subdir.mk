################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Components/ft6x06/ft6x06.c 

OBJS += \
./Core/Components/ft6x06/ft6x06.o 

C_DEPS += \
./Core/Components/ft6x06/ft6x06.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Components/ft6x06/%.o Core/Components/ft6x06/%.su Core/Components/ft6x06/%.cyclo: ../Core/Components/ft6x06/%.c Core/Components/ft6x06/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Components-2f-ft6x06

clean-Core-2f-Components-2f-ft6x06:
	-$(RM) ./Core/Components/ft6x06/ft6x06.cyclo ./Core/Components/ft6x06/ft6x06.d ./Core/Components/ft6x06/ft6x06.o ./Core/Components/ft6x06/ft6x06.su

.PHONY: clean-Core-2f-Components-2f-ft6x06

