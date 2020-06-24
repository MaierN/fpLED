################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include USB_DEVICE/Target/subdir.mk
-include USB_DEVICE/App/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Src/subdir.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
test-usb-storage.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
test-usb-storage.list \

OBJCOPY_BIN += \
test-usb-storage.bin \


# All Target
all: test-usb-storage.elf secondary-outputs

# Tool invocations
test-usb-storage.elf: $(OBJS) $(USER_OBJS) C:\Users\Nicolas\Documents\my_documents\tb\tb_nm\tests_technologiques\stm32_workspace\test-usb-storage\STM32F103C6TX_FLASH.ld
	arm-none-eabi-gcc -o "test-usb-storage.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"C:\Users\Nicolas\Documents\my_documents\tb\tb_nm\tests_technologiques\stm32_workspace\test-usb-storage\STM32F103C6TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="test-usb-storage.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

test-usb-storage.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "test-usb-storage.list"
	@echo 'Finished building: $@'
	@echo ' '

test-usb-storage.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "test-usb-storage.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
