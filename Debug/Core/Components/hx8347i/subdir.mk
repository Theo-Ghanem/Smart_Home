################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Components/hx8347i/hx8347i.c 

OBJS += \
./Core/Components/hx8347i/hx8347i.o 

C_DEPS += \
./Core/Components/hx8347i/hx8347i.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Components/hx8347i/%.o Core/Components/hx8347i/%.su Core/Components/hx8347i/%.cyclo: ../Core/Components/hx8347i/%.c Core/Components/hx8347i/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Components-2f-hx8347i

clean-Core-2f-Components-2f-hx8347i:
	-$(RM) ./Core/Components/hx8347i/hx8347i.cyclo ./Core/Components/hx8347i/hx8347i.d ./Core/Components/hx8347i/hx8347i.o ./Core/Components/hx8347i/hx8347i.su

.PHONY: clean-Core-2f-Components-2f-hx8347i
