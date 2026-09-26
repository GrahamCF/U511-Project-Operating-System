  These are the starting files for the bootloader, it's simple and shortly written on purpose because I figured it would easier to make changes and add stuff
rather than rewritting existing code.

main.asm: source code of the bootloader and assembles it at the given address and directs the BIOS. Also has the halting CPU process
main.bin: assembled output of the main file that confirms a standalone boot sector
main.img: 
