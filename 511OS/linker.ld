/* GNU linker script */

ENTRY(_start)

SECTIONS
{
    . = 1M; /* place kernel at 1 Mb */

    .text :
    {
        *(.multiboot)
        *(.text)
    }

    .rodata : /* read only data */
    {
        *(.rodata)
    }

    .data : /* global and static variables */
    {
        *(.data)
    }

    .bss : /* uninitialized variables */
    {
        *(COMMON)
        *(.bss)
    }
}

