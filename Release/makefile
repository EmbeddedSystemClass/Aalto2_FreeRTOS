################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /opt/ti/ccsv6/tools/compiler/arm_5.1.6

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./subsystems/FreeRTOS-Plus-CLI/FreeRTOS_CLI.obj" \
"./subsystems/cli/cli.obj" \
"./subsystems/test/test.obj" \
"./system/source/system.obj" \
"./system/source/sys_vim.obj" \
"./system/source/sys_startup.obj" \
"./system/source/sys_selftest.obj" \
"./system/source/sys_pmu.obj" \
"./system/source/sys_pmm.obj" \
"./system/source/sys_phantom.obj" \
"./system/source/sys_pcr.obj" \
"./system/source/sys_mpu.obj" \
"./system/source/sys_main.obj" \
"./system/source/sys_intvecs.obj" \
"./system/source/sys_dma.obj" \
"./system/source/sys_core.obj" \
"./system/source/spi.obj" \
"./system/source/sci.obj" \
"./system/source/pom.obj" \
"./system/source/pinmux.obj" \
"./system/source/os_timer.obj" \
"./system/source/os_tasks.obj" \
"./system/source/os_queue.obj" \
"./system/source/os_portasm.obj" \
"./system/source/os_port.obj" \
"./system/source/os_mpu_wrappers.obj" \
"./system/source/os_list.obj" \
"./system/source/os_heap.obj" \
"./system/source/os_event_groups.obj" \
"./system/source/os_croutine.obj" \
"./system/source/notification.obj" \
"./system/source/i2c.obj" \
"./system/source/gio.obj" \
"./system/source/esm.obj" \
"./system/source/dabort.obj" \
"./system/source/can.obj" \
-l"rtsv7R4_T_le_v3D16_eabi.lib" \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include system/source/subdir_vars.mk
-include subsystems/test/subdir_vars.mk
-include subsystems/cli/subdir_vars.mk
-include subsystems/FreeRTOS-Plus-CLI/subdir_vars.mk
-include system/source/subdir_rules.mk
-include subsystems/test/subdir_rules.mk
-include subsystems/cli/subdir_rules.mk
-include subsystems/FreeRTOS-Plus-CLI/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Aalto2_FreeRTOS.out \

EXE_OUTPUTS__QUOTED += \
"Aalto2_FreeRTOS.out" \

BIN_OUTPUTS += \
Aalto2_FreeRTOS.hex \

BIN_OUTPUTS__QUOTED += \
"Aalto2_FreeRTOS.hex" \


# All Target
all: Aalto2_FreeRTOS.out

# Tool invocations
Aalto2_FreeRTOS.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"/opt/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --abi=eabi -me -O2 --display_error_number --diag_warning=225 --diag_wrap=off --enum_type=packed -z -m"Aalto2_FreeRTOS.map" --heap_size=0x800 --stack_size=0x800 -i"/opt/ti/ccsv6/tools/compiler/arm_5.1.6/lib" -i"/opt/ti/ccsv6/tools/compiler/arm_5.1.6/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="Aalto2_FreeRTOS_linkInfo.xml" --rom_model -o "Aalto2_FreeRTOS.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

Aalto2_FreeRTOS.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"/opt/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armhex"  -o "Aalto2_FreeRTOS.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "system/source/can.pp" "system/source/esm.pp" "system/source/gio.pp" "system/source/i2c.pp" "system/source/notification.pp" "system/source/os_croutine.pp" "system/source/os_event_groups.pp" "system/source/os_heap.pp" "system/source/os_list.pp" "system/source/os_mpu_wrappers.pp" "system/source/os_port.pp" "system/source/os_queue.pp" "system/source/os_tasks.pp" "system/source/os_timer.pp" "system/source/pinmux.pp" "system/source/pom.pp" "system/source/sci.pp" "system/source/spi.pp" "system/source/sys_dma.pp" "system/source/sys_main.pp" "system/source/sys_pcr.pp" "system/source/sys_phantom.pp" "system/source/sys_pmm.pp" "system/source/sys_selftest.pp" "system/source/sys_startup.pp" "system/source/sys_vim.pp" "system/source/system.pp" "subsystems/test/test.pp" "subsystems/cli/cli.pp" "subsystems/FreeRTOS-Plus-CLI/FreeRTOS_CLI.pp" 
	-$(RM) "system/source/can.obj" "system/source/dabort.obj" "system/source/esm.obj" "system/source/gio.obj" "system/source/i2c.obj" "system/source/notification.obj" "system/source/os_croutine.obj" "system/source/os_event_groups.obj" "system/source/os_heap.obj" "system/source/os_list.obj" "system/source/os_mpu_wrappers.obj" "system/source/os_port.obj" "system/source/os_portasm.obj" "system/source/os_queue.obj" "system/source/os_tasks.obj" "system/source/os_timer.obj" "system/source/pinmux.obj" "system/source/pom.obj" "system/source/sci.obj" "system/source/spi.obj" "system/source/sys_core.obj" "system/source/sys_dma.obj" "system/source/sys_intvecs.obj" "system/source/sys_main.obj" "system/source/sys_mpu.obj" "system/source/sys_pcr.obj" "system/source/sys_phantom.obj" "system/source/sys_pmm.obj" "system/source/sys_pmu.obj" "system/source/sys_selftest.obj" "system/source/sys_startup.obj" "system/source/sys_vim.obj" "system/source/system.obj" "subsystems/test/test.obj" "subsystems/cli/cli.obj" "subsystems/FreeRTOS-Plus-CLI/FreeRTOS_CLI.obj" 
	-$(RM) "system/source/dabort.pp" "system/source/os_portasm.pp" "system/source/sys_core.pp" "system/source/sys_intvecs.pp" "system/source/sys_mpu.pp" "system/source/sys_pmu.pp" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

