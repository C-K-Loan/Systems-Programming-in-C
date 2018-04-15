#include "bitmap.h"

#include <stdint.h>
#include <stdio.h>

static const uint16_t MAGIC_BM = 0x4D42;

static const uint16_t BITS_PER_PIXEL = 24;

#pragma pack(1)

typedef struct {
	uint32_t header_size;
	uint16_t width;
	uint16_t height;
	uint16_t planes;
	uint16_t bits_per_pixel;
} dib_header_t;

typedef struct {
	uint16_t magic;
	uint32_t file_size;
	uint32_t reserved;
	uint32_t data_offset;
	dib_header_t dib_header;
} bmp_header_t;

#pragma pack()

/*
 * True if system is little-endian, false otherwise.
 */
int is_little_endian() {
	int one = 1;
	char *ptr = (char *) &one;
	return *ptr == 1;
}

/*
 * Converts the 16-bit value to little-endian representation.
 */
uint16_t to_le16(uint16_t value) {
	return is_little_endian() ?
			value : ((value & 0x00ff) << 8) | ((value & 0xff00) >> 8);
}

/*
 * Converts the 32-bit value to little-endian representation.
 */
uint32_t to_le32(uint32_t value) {
	return is_little_endian() ?
			value :
			((value & 0x000000ff) << 24) | ((value & 0x0000ff00) << 8)
					| ((value & 0x00ff0000) >> 8) | ((value & 0xff000000) >> 24);
}

void write_bitmap_header(FILE *stream, int width, int height) {
	/* calc size in bytes of a single line including padding */
	int line_size = width * BITS_PER_PIXEL / 8
			+ (4 - ((width * BITS_PER_PIXEL / 8) % 4)) % 4;

	/* create header structure */
	bmp_header_t head = { to_le16(MAGIC_BM), to_le32(
			sizeof(bmp_header_t) + line_size * height), to_le32(0), to_le32(
			sizeof(bmp_header_t)), { to_le32(sizeof(dib_header_t)), to_le16(
			width), to_le16(height), to_le16(1), to_le16(BITS_PER_PIXEL) } };

	/* write data to given stream */
	fwrite(&head, sizeof(bmp_header_t), 1, stream);
}
