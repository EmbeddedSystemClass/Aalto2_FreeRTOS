/*----------------------------------------------------------------------------*/
/* sys_link_freeRTOS.cmd                                                      */
/*                                                                            */
/* (c) Texas Instruments 2009-2013, All rights reserved.                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN (0) */
/* USER CODE END */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */

--retain="*(.intvecs)"

/* USER CODE BEGIN (1) */
/* USER CODE END */

/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */

MEMORY
{
    VECTORS (X)  : origin=0x00000000 length=0x00000020
    KERNEL  (RX) : origin=0x00000020 length=0x00008000 
    FLASH0  (RX) : origin=0x00008020 length=0x00177FE0
    FLASH1  (RX) : origin=0x00180000 length=0x00180000
    STACKS  (RW) : origin=0x08000000 length=0x00001500
    KRAM    (RW) : origin=0x08001500 length=0x00000800
    RAM     (RW) : origin=(0x08001500+0x00000800) length=(0x0003eb00 - 0x00000800)
    
/* USER CODE BEGIN (2) */
/* USER CODE END */
}

/* USER CODE BEGIN (3) */
/* USER CODE END */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .intvecs : {} > VECTORS
    /* FreeRTOS Kernel in protected region of Flash */
    .kernelTEXT   : { sys_startup.obj(.const)
                      os_tasks.obj (.const:.string)
                      -l=rtsv7R4_T_le_v3D16_eabi.lib<auto_init.obj> (.text)
                      -l=rtsv7R4_T_le_v3D16_eabi.lib<copy_decompress_rle.obj> (*)
                      -l=rtsv7R4_T_le_v3D16_eabi.lib<cpy_tbl.obj> (*)
                      -l=rtsv7R4_T_le_v3D16_eabi.lib<copy_zero_init.obj> (*)
                      -l=rtsv7R4_T_le_v3D16_eabi.lib<copy_decompress_none.obj> (*)
                      -l=rtsv7R4_T_le_v3D16_eabi.lib<icall32.obj> (.text)
                      -l=rtsv7R4_T_le_v3D16_eabi.lib<memset32.obj> (.text)
                      -l=rtsv7R4_T_le_v3D16_eabi.lib<memcpy32.obj> (.text)
                    } > KERNEL
    .cinit        : {} > KERNEL
    .pinit        : {} > KERNEL
    /* Rest of code to user mode flash region */
    .text         : {} > FLASH0 | FLASH1
    .const        : {} > FLASH0 | FLASH1
    /* FreeRTOS Kernel data in protected region of RAM */
    .kernelBSS    : {} > KRAM
    .kernelHEAP   : {} > RAM
    .bss          : {} > RAM
    .data         : {} > RAM    

/* USER CODE BEGIN (4) */
/* USER CODE END */
}

/* USER CODE BEGIN (5) */
/* USER CODE END */

/*----------------------------------------------------------------------------*/
/* Misc                                                                       */

/* USER CODE BEGIN (6) */
/* USER CODE END */

/*----------------------------------------------------------------------------*/

