#include "print_text.h"
#include "font8x8.h"

#define txtfont font8x8_basic

#define PIXELS_BY_RECT 8
#define MAX 255

struct rect {
	uint16_t x;
	uint16_t y;
}; struct rect cursor = (struct rect){0, 0};

static struct limine_framebuffer *s_fb;
#define access_pixel(x, y) ((y) * ((s_fb)->pitch / 4) + (x)) 
void gfx_init()
{
	// Ensure the bootloader actually understands our base revision (see spec).
	if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
		hcf();
	}

	// Ensure we got a framebuffer.
	if (framebuffer_request.response == NULL
	 || framebuffer_request.response->framebuffer_count < 1) {
		hcf();
	}

	for (uint64_t i = 0; i < framebuffer_request.response->framebuffer_count; i++) {
		s_fb = framebuffer_request.response->framebuffers[i];

		// Ensure the framebuffer has 32-bit RGB pixels, the only kind we handle.
		if (s_fb->memory_model != LIMINE_FRAMEBUFFER_RGB || s_fb->bpp != 32) {
			hcf();
		}
	}
}

static inline void print_char_inline(char c)
{
	volatile uint32_t *fb_ptr = s_fb->address;
	uint32_t green = fb_pixel(s_fb, 0, MAX, 0);

	if (c != '\n') {
		for (uint8_t i = 0; i < PIXELS_BY_RECT; i++) { //y
			for (uint8_t j = 0; j < PIXELS_BY_RECT; j++) { //x
				if (1 << j & txtfont[(unsigned char)c][i])
					fb_ptr[access_pixel(cursor.x * PIXELS_BY_RECT + j, cursor.y * PIXELS_BY_RECT + i)] = green;
			}
		}
	} else {
		cursor.x = 0;
		cursor.y++;
		return;
	}
	if (cursor.x > 500) {
		cursor.x = 0;
		cursor.y++;
	}
	if (cursor.y > 500) {
		cursor.x = 0;
		cursor.y = 0;
	}
	cursor.x++;
} void print_char(char c) { print_char_inline(c); }

static inline void print_string_inline(string line)
{
	for (size_t i = 0; i < line.size; i++) {
		print_char_inline(line.data[i]);
	}
} void print_string(string line) { print_string_inline(line); };

static inline void print_str_inline(char *line)
{
	// TODO: remove this later!
	if (cursor.y > 200) return;
	for (size_t i = 0; line[i] != '\0'; i++) {
		print_char_inline(line[i]);
	}	
} void print_line(char *line) { print_str_inline(line); };


void print_hex(uint64_t val)
{
	char hex_chars[] = "0123456789ABCDEF";
	char buf[19];
	buf[0] = '0';
	buf[1] = 'x';
	for (int i = 15; i >= 0; i--) {
		buf[2 + (15 - i)] = hex_chars[(val >> (i * 4)) & 0xF];
	}
	buf[18] = '\0';
	print_line(buf);
}
