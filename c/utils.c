#include <stddef.h>
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
//static const size_t VGA_HEIGHT = 25;
static size_t terminal_row;
static size_t terminal_column;
static uint16_t* terminal_buffer;
static uint8_t terminal_color;


extern int vga_entry(int uc, int color);
extern char vga_entry_color(int fg, int bg);
extern double terminal_putchar(char c);

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
void write_to_screen(char c, int index, uint8_t terminal_color /*, size_t terminal_column, size_t terminal_row*/){
    terminal_buffer = (uint16_t*) 0xB8000;
	//size_t index_c = terminal_row * VGA_WIDTH + terminal_column;
    terminal_buffer[index] = vga_entry(c, terminal_color);
}
/*void write_to_screen_complete(char* data){
	terminal_row = 0;
	terminal_column = 0;
	//terminal_color = 7 | 0 << 4;
	terminal_color = vga_entry_color(7, 0);
	terminal_buffer = (uint16_t*) 0xB8000;
	size_t size = strlen(data);
	for (size_t i = 0; i < size; i++){
		const size_t index = terminal_row * VGA_WIDTH + terminal_column;
		terminal_buffer[index] = (uint16_t) data[i] | (uint16_t) terminal_color << 8;
		terminal_column++;
	}
	
}*/


void write_to_buffer(uint16_t index, uint16_t* data, int i, uint8_t terminal_col){
    terminal_buffer[index] = (uint16_t) data[i] | (uint16_t) terminal_col << 8;
}

/*uint16_t getting_data_to_write(char* data, uint16_t terminal_col, int pos){
    return (uint16_t) data[pos] | (uint16_t) terminal_col << 8;
}*/

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" :: "a" (value), "dN" (port));
}

uint8_t inb (uint16_t port) {
    uint8_t result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

#define CR4_OSFXSR (1 << 9)
#define CR4_OSXMMEXCPT (1 << 10)
#define CR0_MP (1 << 1)
#define CR0_EM (1 << 2)

void fpu_init() {
    uint32_t cr;
    /* Configure CR0: disable emulation (EM), as we assume we have an FPU, and
     * enable the EM bit: with the TS and EM bits disabled, `wait/fwait`
     * instructions won't generate exceptions. This last part is desired
     * because we save the FPU state on each task switch. */
    asm volatile(
        "clts\n"
        "mov %%cr0, %0" : "=r"(cr));
    cr &= ~CR0_EM;
    cr |= CR0_MP;
    asm volatile("mov %0, %%cr0" ::"r"(cr));
    /* Configure CR4: enable the `fxsave` and `fxrstor` instructions, along
     * with SSE, and SSE-related exception handling. */
    asm volatile("mov %%cr4, %0" : "=r"(cr));
    cr |= CR4_OSFXSR | CR4_OSXMMEXCPT;

    asm volatile(
        "mov %0, %%cr4\n"
        "fninit" ::"r"(cr));

}


struct gdt_entry
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access;
    unsigned char granularity;
    unsigned char base_high;
} __attribute__((packed));

extern struct gdt_entry gdt[6];

void set_gdt_entry(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity){
    gdt[index].base_low = (base & 0xFFFF);
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;
    gdt[index].limit_low = (limit & 0xFFFF);
    gdt[index].granularity = ((limit >> 16) & 0x0F);
    gdt[index].granularity |= (granularity & 0xF0);
    gdt[index].access = access;
}