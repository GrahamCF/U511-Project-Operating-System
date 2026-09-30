// Direct Port I/O Helpers
static inline unsigned char inb(unsigned short port)
{
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data)
{
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// VGA Text Mode Buffer Helpers
static volatile unsigned short* const VGA_MEMORY = (unsigned short*)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;

void print_char(char c, unsigned char color = 0x07)
{
    if (c == '\n')
    {
        cursor_x = 0;
        cursor_y++;
    }
    else
    {
        int index = cursor_y * 80 + cursor_x;
        VGA_MEMORY[index] = ((unsigned short)color << 8) | c;
        cursor_x++;
        if (cursor_x >= 80)
        {
            cursor_x = 0;
            cursor_y++;
        }
    }
}

void print_string(const char* str, unsigned char color = 0x07)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        print_char(str[i], color);
    }
}

// GDT Structure
struct GDTEntry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
} __attribute__((packed));

struct GDTPtr
{
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed));

GDTEntry gdt[3];
GDTPtr gdt_ptr;

void gdt_set_gate(int num, unsigned int base, unsigned int limit, unsigned char access, unsigned char gran)
{
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

void init_gdt()
{
    gdt_ptr.limit = (sizeof(GDTEntry) * 3) - 1;
    gdt_ptr.base  = (unsigned int)&gdt;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment

    __asm__ volatile(
        "lgdt %0\n\t"
        "mov $0x10, %%ax\n\t"
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "mov %%ax, %%fs\n\t"
        "mov %%ax, %%gs\n\t"
        "mov %%ax, %%ss\n\t"
        "ljmp $0x08, $1f\n\t"
        "1:\n\t"
        : : "m"(gdt_ptr) : "ax"
    );
}

// IDT Structure
struct IDTEntry
{
    unsigned short base_low;
    unsigned short selector;
    unsigned char  always0;
    unsigned char  flags;
    unsigned short base_high;
} __attribute__((packed));

struct IDTPtr
{
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed));

IDTEntry idt[256];
IDTPtr idt_ptr;

void idt_set_gate(unsigned char num, unsigned int base, unsigned short selector, unsigned char flags)
{
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = selector;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

// Remap PIC to avoid conflict with CPU exceptions (offset IRQs to 0x20 - 0x2F)
void init_pic()
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20); // Master PIC vector offset (32)
    outb(0xA1, 0x28); // Slave PIC vector offset (40)
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    
    // Mask all interrupts except IRQ1 (Keyboard)
    outb(0x21, 0xFD); // 0b11111101 -> Enables IRQ1
    outb(0xA1, 0xFF);
}

// Scan code set 1 lookup table for standard key presses
static const char scancode_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
   '*',  0, ' '
};

// Keyboard Interrupt Handler (IRQ1 -> Vector 33 / 0x21)
extern "C" __attribute__((interrupt)) void keyboard_handler(void* frame)
{
    unsigned char scancode = inb(0x60);

    // If top bit is 0, it's a key press event (not release)
    if (!(scancode & 0x80))
    {
        if (scancode < sizeof(scancode_ascii))
        {
            char c = scancode_ascii[scancode];
            if (c != 0)
            {
                print_char(c, 0x0A); // Print typed character in green
            }
        }
    }

    // Send End of Interrupt (EOI) to Master PIC
    outb(0x20, 0x20);
}

void init_idt()
{
    idt_ptr.limit = sizeof(IDTEntry) * 256 - 1;
    idt_ptr.base  = (unsigned int)&idt;

    // Set Keyboard handler at IRQ1 (Interrupt vector 33 / 0x21)
    idt_set_gate(33, (unsigned int)keyboard_handler, 0x08, 0x8E);

    __asm__ volatile ("lidt %0" : : "m"(idt_ptr));
}

// Kernel Main Entry
extern "C" void kernel_main()
{
    init_gdt();
    init_pic();
    init_idt();

    print_string("Operating Systems U511 Kernel\n", 0x07);
    print_string("Type something: ", 0x0E);

    // Re-enable hardware interrupts
    __asm__ volatile ("sti");

    while (true)
    {
        __asm__ volatile ("hlt");
    }
}