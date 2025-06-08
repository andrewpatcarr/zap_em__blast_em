################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc_task.c \
../Core/Src/audio_data.c \
../Core/Src/controller_task.c \
../Core/Src/encoder_driver.c \
../Core/Src/game_task.c \
../Core/Src/lcd.c \
../Core/Src/main.c \
../Core/Src/motor_driver.c \
../Core/Src/photoresistor_task.c \
../Core/Src/shoot_task.c \
../Core/Src/sound_task.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/adc_task.o \
./Core/Src/audio_data.o \
./Core/Src/controller_task.o \
./Core/Src/encoder_driver.o \
./Core/Src/game_task.o \
./Core/Src/lcd.o \
./Core/Src/main.o \
./Core/Src/motor_driver.o \
./Core/Src/photoresistor_task.o \
./Core/Src/shoot_task.o \
./Core/Src/sound_task.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/adc_task.d \
./Core/Src/audio_data.d \
./Core/Src/controller_task.d \
./Core/Src/encoder_driver.d \
./Core/Src/game_task.d \
./Core/Src/lcd.d \
./Core/Src/main.d \
./Core/Src/motor_driver.d \
./Core/Src/photoresistor_task.d \
./Core/Src/shoot_task.d \
./Core/Src/sound_task.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc_task.cyclo ./Core/Src/adc_task.d ./Core/Src/adc_task.o ./Core/Src/adc_task.su ./Core/Src/audio_data.cyclo ./Core/Src/audio_data.d ./Core/Src/audio_data.o ./Core/Src/audio_data.su ./Core/Src/controller_task.cyclo ./Core/Src/controller_task.d ./Core/Src/controller_task.o ./Core/Src/controller_task.su ./Core/Src/encoder_driver.cyclo ./Core/Src/encoder_driver.d ./Core/Src/encoder_driver.o ./Core/Src/encoder_driver.su ./Core/Src/game_task.cyclo ./Core/Src/game_task.d ./Core/Src/game_task.o ./Core/Src/game_task.su ./Core/Src/lcd.cyclo ./Core/Src/lcd.d ./Core/Src/lcd.o ./Core/Src/lcd.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/motor_driver.cyclo ./Core/Src/motor_driver.d ./Core/Src/motor_driver.o ./Core/Src/motor_driver.su ./Core/Src/photoresistor_task.cyclo ./Core/Src/photoresistor_task.d ./Core/Src/photoresistor_task.o ./Core/Src/photoresistor_task.su ./Core/Src/shoot_task.cyclo ./Core/Src/shoot_task.d ./Core/Src/shoot_task.o ./Core/Src/shoot_task.su ./Core/Src/sound_task.cyclo ./Core/Src/sound_task.d ./Core/Src/sound_task.o ./Core/Src/sound_task.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

