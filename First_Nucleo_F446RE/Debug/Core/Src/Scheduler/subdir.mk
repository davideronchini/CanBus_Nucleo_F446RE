################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Scheduler/Scheduler.c 

OBJS += \
./Core/Src/Scheduler/Scheduler.o 

C_DEPS += \
./Core/Src/Scheduler/Scheduler.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Scheduler/%.o Core/Src/Scheduler/%.su Core/Src/Scheduler/%.cyclo: ../Core/Src/Scheduler/%.c Core/Src/Scheduler/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/dadol/Documents/School/Formula SAE/STM32/CanBus_Nucleo_F446RE/First_Nucleo_F446RE/Core/Src/Tasks" -I"C:/Users/dadol/Documents/School/Formula SAE/STM32/CanBus_Nucleo_F446RE/First_Nucleo_F446RE/Core/Src/Scheduler" -I"C:/Users/dadol/Documents/School/Formula SAE/STM32/CanBus_Nucleo_F446RE/First_Nucleo_F446RE/Core/Src/Can" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Scheduler

clean-Core-2f-Src-2f-Scheduler:
	-$(RM) ./Core/Src/Scheduler/Scheduler.cyclo ./Core/Src/Scheduler/Scheduler.d ./Core/Src/Scheduler/Scheduler.o ./Core/Src/Scheduler/Scheduler.su

.PHONY: clean-Core-2f-Src-2f-Scheduler
