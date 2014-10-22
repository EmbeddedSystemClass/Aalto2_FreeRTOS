################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../system/source/dabort.asm \
../system/source/os_portasm.asm \
../system/source/sys_core.asm \
../system/source/sys_intvecs.asm \
../system/source/sys_mpu.asm \
../system/source/sys_pmu.asm 

C_SRCS += \
../system/source/can.c \
../system/source/esm.c \
../system/source/gio.c \
../system/source/i2c.c \
../system/source/notification.c \
../system/source/os_croutine.c \
../system/source/os_event_groups.c \
../system/source/os_heap.c \
../system/source/os_list.c \
../system/source/os_mpu_wrappers.c \
../system/source/os_port.c \
../system/source/os_queue.c \
../system/source/os_tasks.c \
../system/source/os_timer.c \
../system/source/pinmux.c \
../system/source/pom.c \
../system/source/sci.c \
../system/source/spi.c \
../system/source/sys_dma.c \
../system/source/sys_main.c \
../system/source/sys_pcr.c \
../system/source/sys_phantom.c \
../system/source/sys_pmm.c \
../system/source/sys_selftest.c \
../system/source/sys_startup.c \
../system/source/sys_vim.c \
../system/source/system.c 

OBJS += \
./system/source/can.obj \
./system/source/dabort.obj \
./system/source/esm.obj \
./system/source/gio.obj \
./system/source/i2c.obj \
./system/source/notification.obj \
./system/source/os_croutine.obj \
./system/source/os_event_groups.obj \
./system/source/os_heap.obj \
./system/source/os_list.obj \
./system/source/os_mpu_wrappers.obj \
./system/source/os_port.obj \
./system/source/os_portasm.obj \
./system/source/os_queue.obj \
./system/source/os_tasks.obj \
./system/source/os_timer.obj \
./system/source/pinmux.obj \
./system/source/pom.obj \
./system/source/sci.obj \
./system/source/spi.obj \
./system/source/sys_core.obj \
./system/source/sys_dma.obj \
./system/source/sys_intvecs.obj \
./system/source/sys_main.obj \
./system/source/sys_mpu.obj \
./system/source/sys_pcr.obj \
./system/source/sys_phantom.obj \
./system/source/sys_pmm.obj \
./system/source/sys_pmu.obj \
./system/source/sys_selftest.obj \
./system/source/sys_startup.obj \
./system/source/sys_vim.obj \
./system/source/system.obj 

ASM_DEPS += \
./system/source/dabort.pp \
./system/source/os_portasm.pp \
./system/source/sys_core.pp \
./system/source/sys_intvecs.pp \
./system/source/sys_mpu.pp \
./system/source/sys_pmu.pp 

C_DEPS += \
./system/source/can.pp \
./system/source/esm.pp \
./system/source/gio.pp \
./system/source/i2c.pp \
./system/source/notification.pp \
./system/source/os_croutine.pp \
./system/source/os_event_groups.pp \
./system/source/os_heap.pp \
./system/source/os_list.pp \
./system/source/os_mpu_wrappers.pp \
./system/source/os_port.pp \
./system/source/os_queue.pp \
./system/source/os_tasks.pp \
./system/source/os_timer.pp \
./system/source/pinmux.pp \
./system/source/pom.pp \
./system/source/sci.pp \
./system/source/spi.pp \
./system/source/sys_dma.pp \
./system/source/sys_main.pp \
./system/source/sys_pcr.pp \
./system/source/sys_phantom.pp \
./system/source/sys_pmm.pp \
./system/source/sys_selftest.pp \
./system/source/sys_startup.pp \
./system/source/sys_vim.pp \
./system/source/system.pp 

C_DEPS__QUOTED += \
"system/source/can.pp" \
"system/source/esm.pp" \
"system/source/gio.pp" \
"system/source/i2c.pp" \
"system/source/notification.pp" \
"system/source/os_croutine.pp" \
"system/source/os_event_groups.pp" \
"system/source/os_heap.pp" \
"system/source/os_list.pp" \
"system/source/os_mpu_wrappers.pp" \
"system/source/os_port.pp" \
"system/source/os_queue.pp" \
"system/source/os_tasks.pp" \
"system/source/os_timer.pp" \
"system/source/pinmux.pp" \
"system/source/pom.pp" \
"system/source/sci.pp" \
"system/source/spi.pp" \
"system/source/sys_dma.pp" \
"system/source/sys_main.pp" \
"system/source/sys_pcr.pp" \
"system/source/sys_phantom.pp" \
"system/source/sys_pmm.pp" \
"system/source/sys_selftest.pp" \
"system/source/sys_startup.pp" \
"system/source/sys_vim.pp" \
"system/source/system.pp" 

OBJS__QUOTED += \
"system/source/can.obj" \
"system/source/dabort.obj" \
"system/source/esm.obj" \
"system/source/gio.obj" \
"system/source/i2c.obj" \
"system/source/notification.obj" \
"system/source/os_croutine.obj" \
"system/source/os_event_groups.obj" \
"system/source/os_heap.obj" \
"system/source/os_list.obj" \
"system/source/os_mpu_wrappers.obj" \
"system/source/os_port.obj" \
"system/source/os_portasm.obj" \
"system/source/os_queue.obj" \
"system/source/os_tasks.obj" \
"system/source/os_timer.obj" \
"system/source/pinmux.obj" \
"system/source/pom.obj" \
"system/source/sci.obj" \
"system/source/spi.obj" \
"system/source/sys_core.obj" \
"system/source/sys_dma.obj" \
"system/source/sys_intvecs.obj" \
"system/source/sys_main.obj" \
"system/source/sys_mpu.obj" \
"system/source/sys_pcr.obj" \
"system/source/sys_phantom.obj" \
"system/source/sys_pmm.obj" \
"system/source/sys_pmu.obj" \
"system/source/sys_selftest.obj" \
"system/source/sys_startup.obj" \
"system/source/sys_vim.obj" \
"system/source/system.obj" 

ASM_DEPS__QUOTED += \
"system/source/dabort.pp" \
"system/source/os_portasm.pp" \
"system/source/sys_core.pp" \
"system/source/sys_intvecs.pp" \
"system/source/sys_mpu.pp" \
"system/source/sys_pmu.pp" 

C_SRCS__QUOTED += \
"../system/source/can.c" \
"../system/source/esm.c" \
"../system/source/gio.c" \
"../system/source/i2c.c" \
"../system/source/notification.c" \
"../system/source/os_croutine.c" \
"../system/source/os_event_groups.c" \
"../system/source/os_heap.c" \
"../system/source/os_list.c" \
"../system/source/os_mpu_wrappers.c" \
"../system/source/os_port.c" \
"../system/source/os_queue.c" \
"../system/source/os_tasks.c" \
"../system/source/os_timer.c" \
"../system/source/pinmux.c" \
"../system/source/pom.c" \
"../system/source/sci.c" \
"../system/source/spi.c" \
"../system/source/sys_dma.c" \
"../system/source/sys_main.c" \
"../system/source/sys_pcr.c" \
"../system/source/sys_phantom.c" \
"../system/source/sys_pmm.c" \
"../system/source/sys_selftest.c" \
"../system/source/sys_startup.c" \
"../system/source/sys_vim.c" \
"../system/source/system.c" 

ASM_SRCS__QUOTED += \
"../system/source/dabort.asm" \
"../system/source/os_portasm.asm" \
"../system/source/sys_core.asm" \
"../system/source/sys_intvecs.asm" \
"../system/source/sys_mpu.asm" \
"../system/source/sys_pmu.asm" 


