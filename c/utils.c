#include <stddef.h>
#include <stdint.h>

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
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
void write_to_screen(char c, int index, uint8_t terminal_color, size_t terminal_column, size_t terminal_row){
    terminal_buffer = (uint16_t*) 0xB8000;
	size_t index_c = terminal_row * VGA_WIDTH + terminal_column;
    terminal_buffer[index] = vga_entry(c, terminal_color);
}
void write_to_screen_complete(char* data){
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
	
}

uint16_t getting_data_to_write(char* data, uint16_t terminal_col, int pos){
    return (uint16_t) data[pos] | (uint16_t) terminal_col << 8;
}

void terminal_write(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}