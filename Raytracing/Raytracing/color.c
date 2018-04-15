#include "color.h"

#include <stdio.h>

#include "math.h"

void col_print(col_t *col) {
	printf("r: %f, g: %f, b: %f\n", col->r, col->g, col->b);
}

void pix_print(pix_t *pix) {
	printf("r: %i, g: %i, b: %i\n", pix->r, pix->g, pix->b);
}

pix_t col2pix(col_t *col) {
	pix_t c;
	c.r = (uint8_t) max(0.0, min(255.0, (col->r * 255.0)));
	c.g = (uint8_t) max(0.0, min(255.0, (col->g * 255.0)));
	c.b = (uint8_t) max(0.0, min(255.0, (col->b * 255.0)));

	return c;
}
