################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
subsystems/main/main.obj: ../subsystems/main/main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --abi=eabi -me -Ooff --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/main" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/TaskManaging" --include_path="/opt/ti/ccsv6/tools/compiler/arm_5.1.6/include" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/FreeRTOS-Plus-FAT-SL/psp/target/fat_sl" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/FreeRTOS-Plus-FAT-SL/api" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/FreeRTOS-Plus-FAT-SL/config" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/FreeRTOS-Plus-FAT-SL/media-drv/ram" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/FreeRTOS-Plus-FAT-SL" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/cli" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/subsystems/FreeRTOS-Plus-CLI" --include_path="/mnt/74a0dd9d-6f56-4a73-83a6-b42e06457218/home/nemanja/workplace/Aalto2/Aalto2_FreeRTOS/system/include" -g --display_error_number --diag_warning=225 --diag_wrap=off --enum_type=packed --preproc_with_compile --preproc_dependency="subsystems/main/main.pp" --obj_directory="subsystems/main" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


