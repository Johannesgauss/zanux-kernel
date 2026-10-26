#ifndef PRINT_TEXT_H
#define PRINT_TEXT_H
#include <limine.h>
#include "graphical_limine_util.h"

typedef struct string {
	char *data;	
	size_t size;
} string;

static inline string string_init(char *data, size_t size)
{
	string self;
	self.data = data;
	self.size = size;

	return self;
}
void gfx_init();

void print_char(char c);
void print_string(string line);
void kprint(char *line);
void print_line(char *line);
void print_hex(uint64_t val);

#endif // PRINT_TEXT_H
