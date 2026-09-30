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
