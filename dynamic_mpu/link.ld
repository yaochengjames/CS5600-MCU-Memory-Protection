/* Linker script for QEMU MPS2-AN386 (Cortex-M4) */
/* FLASH at 0x00000000, SRAM at 0x20000000 */

ENTRY(Reset_Handler)

/* QEMU MPS2: 2MB Flash, 512KB RAM */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 2M
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 512K
}

/* Stack top (used by startup code) */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  /* Interrupt vector table */
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  /* Program code */
  .text :
  {
    . = ALIGN(4);
    *(.text*)
    *(.init) *(.fini)
    *(.glue_7) *(.glue_7t)
    *(.eh_frame*)
    . = ALIGN(4);
  } > FLASH

  /* Read-only data */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  /* Exception table (used by GCC) */
  .ARM.extab :  { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
  .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } > FLASH

  /* End of FLASH, used to load .data */
  _etext  = .;
  _sidata = _etext;

  /* Initialized data (.data) runs in RAM, loaded from FLASH */
  .data : AT ( _sidata )
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM

  /* Zero-initialized data (.bss) in RAM */
  .bss (NOLOAD) :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM
}

