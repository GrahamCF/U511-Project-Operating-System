// IDT Structure
struct IDTEntry // Entry in IDT
{
    unsigned short base_low; // store lower 16 bits of address
    unsigned short selector; // for GDT 
    unsigned char  always0; // reserved byte
    unsigned char  flags; // attributes of interrupt
    unsigned short base_high; // store upper 16 bits of address
} __attribute__((packed)); // layout of 8 bytes

// Where IDT is located (IDT Pointer)
struct IDTPtr
{
    unsigned short limit; // find how large IDT is (sizeIDT - 1)
    unsigned int   base; // memory address of IDT
} __attribute__((packed));

static inline unsigned char inb(unsigned short port) // return 1 byte from I/O port
{
    unsigned char result; // stores info from I/O port
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port)); // assembly code to read byte
    return result; // return byte
}

// Scan code set 1 lookup table for standard key presses, used to translate keyboard scan into characters using ascii
static const char scancode_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
   '*',  0, ' '
}; // does not account for shift key being held

// Basic lowercase keyboard handler
extern "C" __attribute__((interrupt)) void keyboard_handler(void* frame)
{
    unsigned char scancode = inb(0x60); // read scan code from keyboard port

    if (!(scancode & 0x80)) // If top bit is 0, it's a key press event (not release)
    {
        if (scancode < sizeof(scancode_ascii))
        {
            char c = scancode_ascii[scancode]; // convert keyboard scan into a character using ascii
        }
    }
}

extern "C" void kernel_main() // Use C
{
    volatile unsigned short* video = // hardware updates memory
    // each character takes up 2 bytes
        (unsigned short*)0xB8000; // Memory address for color text-mode video memory
    // String to display
    const char* message = "Operating Systems U511 Kernel";
    // loop through each character
    for (int i = 0; message[i] != '\0'; i++)
    {
        video[i] = (0x07 << 8) | message[i]; // write value to video memory buffer
    }

    while (true) // stop kernel from exiting
    {
    }
}
