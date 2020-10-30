################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/FTM.c \
../source/PORT.c \
../source/gpio.c \
../source/hardware.c \
../source/main.c \
../source/uart.c 

OBJS += \
./source/FTM.o \
./source/PORT.o \
./source/gpio.o \
./source/hardware.o \
./source/main.o \
./source/uart.o 

C_DEPS += \
./source/FTM.d \
./source/PORT.d \
./source/gpio.d \
./source/hardware.d \
./source/main.d \
./source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCR_INTEGER_PRINTF -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DCPU_MK64FN1M0VDC12 -DCPU_MK64FN1M0VDC12_cm4 -D__REDLIB__ -I"C:\Users\marth\Documents\ITBA\Labo de micros\Tps\Tp2\TP2\FTM_PWM\source" -I"C:\Users\marth\Documents\ITBA\Labo de micros\Tps\Tp2\TP2\FTM_PWM" -I"C:\Users\marth\Documents\ITBA\Labo de micros\Tps\Tp2\TP2\FTM_PWM\startup" -I"C:\Users\marth\Documents\ITBA\Labo de micros\Tps\Tp2\TP2\FTM_PWM\CMSIS" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -Wa,-adhlns="$@.lst" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


