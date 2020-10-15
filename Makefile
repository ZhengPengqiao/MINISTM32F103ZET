RM := rm -rf
LIBS :=
C_SRCS := 
CPP_SRCS := 

INCLUDES := \
	-I"./include" \
	-I"./MyModule/include" \
	-I"./system/include" \
	-I"./system/include/cmsis" \
	-I"./system/include/stm32f1-stdperiph"

# ./system/src/stm32f1-stdperiph/
C_SRCS += \
./system/src/stm32f1-stdperiph/misc.c \
./system/src/stm32f1-stdperiph/stm32f10x_adc.c \
./system/src/stm32f1-stdperiph/stm32f10x_bkp.c \
./system/src/stm32f1-stdperiph/stm32f10x_dma.c \
./system/src/stm32f1-stdperiph/stm32f10x_exti.c \
./system/src/stm32f1-stdperiph/stm32f10x_fsmc.c \
./system/src/stm32f1-stdperiph/stm32f10x_gpio.c \
./system/src/stm32f1-stdperiph/stm32f10x_i2c.c \
./system/src/stm32f1-stdperiph/stm32f10x_pwr.c \
./system/src/stm32f1-stdperiph/stm32f10x_rcc.c \
./system/src/stm32f1-stdperiph/stm32f10x_rtc.c \
./system/src/stm32f1-stdperiph/stm32f10x_sdio.c \
./system/src/stm32f1-stdperiph/stm32f10x_tim.c \
./system/src/stm32f1-stdperiph/stm32f10x_usart.c 

# ./system/src/newlib/
C_SRCS += \
./system/src/newlib/_exit.c \
./system/src/newlib/_sbrk.c \
./system/src/newlib/_startup.c \
./system/src/newlib/_syscalls.c \
./system/src/newlib/assert.c 

CPP_SRCS += \
./system/src/newlib/_cxx.cpp 

# ./system/src/diag/
C_SRCS += \
./system/src/diag/Trace.c \
./system/src/diag/trace_impl.c 

# ./system/src/cortexm/
C_SRCS += \
./system/src/cortexm/_initialize_hardware.c \
./system/src/cortexm/_reset_hardware.c \
./system/src/cortexm/exception_handlers.c 


# ./system/src/cmsis/
C_SRCS += \
./system/src/cmsis/system_stm32f10x.c \
./system/src/cmsis/vectors_stm32f10x.c 

# ./src/
C_SRCS += \
./src/main.c 

# ./MyModule/src/
C_SRCS += \
./MyModule/src/Adc.c \
./MyModule/src/Delay.c \
./MyModule/src/Key.c \
./MyModule/src/Lcd.c \
./MyModule/src/Led.c \
./MyModule/src/PWM.c \
./MyModule/src/cc936.c \
./MyModule/src/diskio.c \
./MyModule/src/ff.c \
./MyModule/src/inv_mpu.c \
./MyModule/src/inv_mpu_dmp_motion_driver.c \
./MyModule/src/mpu6050.c \
./MyModule/src/mpuiic.c \
./MyModule/src/rtc.c \
./MyModule/src/sdio_sdcard.c \
./MyModule/src/sys.c \
./MyModule/src/touch.c \
./MyModule/src/usart.c 

C_OBJS  := $(C_SRCS:.c=.o)
CPP_OBJS  += $(CPP_SRCS:.cpp=.o)

C_DEPS := $(C_SRCS:.c=.d)
CPP_DEPS += $(CPP_SRCS:.cpp=.d)



SECONDARY_FLASH := MINISTM32F103ZET.bin
SECONDARY_SIZE := MINISTM32F103ZET.siz


# All Target
all: MINISTM32F103ZET.elf secondary-outputs 

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

	
$(C_OBJS):%.o:%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 $(INCLUDES) -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

$(CPP_OBJS):%.o:%.cpp  
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG -DSTM32F10X_HD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 $(INCLUDES) -std=gnu++11 -fabi-version=0 -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
	
# Tool invocations
MINISTM32F103ZET.elf: $(C_OBJS) $(CPP_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -T mem.ld -T libs.ld -T sections.ld -nostartfiles -Xlinker --gc-sections -L"./ldscripts" -Wl,-Map,"MINISTM32F103ZET.map" --specs=nano.specs -o "MINISTM32F103ZET.elf" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

MINISTM32F103ZET.bin: MINISTM32F103ZET.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O binary "MINISTM32F103ZET.elf"  "MINISTM32F103ZET.bin"
	@echo 'Finished building: $@'
	@echo ' '

MINISTM32F103ZET.siz: MINISTM32F103ZET.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "MINISTM32F103ZET.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_OBJS) $(CPP_OBJS) $(CPP_DEPS) $(C_DEPS) $(SECONDARY_FLASH) $(SECONDARY_SIZE) MINISTM32F103ZET.elf MINISTM32F103ZET.map
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

