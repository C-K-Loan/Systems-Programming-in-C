#ifndef COLOR_H_
#define COLOR_H_

#include <stdint.h>

typedef struct {
	uint8_t b;
	uint8_t g;
	uint8_t r;
} pix_t;

typedef struct {
	float r;
	float g;
	float b;
} col_t;

void col_print(col_t *col);

void pix_print(pix_t *pix);

pix_t col2pix(col_t *col);

#endif
