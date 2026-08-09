/* Specifying the firmware's entry point */
ENTRY(Reset_Handler)

/* Detailing the available memory */
MEMORY
{
    FLASH(rx) :ORIGIN =0x08000000, LENGTH =1024K
    SRAM(rwx) :ORIGIN =0x20000000, LENGTH =128K
}

_estack = ORIGIN(SRAM)+LENGTH(SRAM);

/* Specifying the necessary heap and stack sizes */

__max_heap_size = 0x200;
__max_stack_size = 0x400;

/* Defining output sections */
SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        *(.isr_vector_tbl)
        *(.text)
        *(.rodata)
        . = ALIGN(4);
        _etext = .;
    } > FLASH
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT> FLASH /* >(vma) AT> (lma) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        . = ALIGN(4);
        *(.bss)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM
}