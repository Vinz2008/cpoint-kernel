#include <stddef.h>
#include <stdint.h>

int vga_entry(int uc, int color);
size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
void write_to_screen(uint16_t* buffer, char c, int index, uint8_t terminal_color){
    uint16_t* buf = (uint16_t*)0xB8000; 
    buf[index] = vga_entry(c, terminal_color);
}