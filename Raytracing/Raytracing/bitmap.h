#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdio.h>

/*
 * Bitmap header size in bytes.
 */
static const int BITMAP_HEADER_SIZE = 26;

/*
 * Writes a header for the windows bitmap format (BMP) to the given stream.
 *
 * The type of the data is a 24-bit RGB format where every pixel is defined by
 * three 8-bit values for blue, green and red. The data after the header is
 * written row-wise starting with the last row.
 * width and height have to be completely divisible by 4.
 *
 * width : the width of the bitmap in pixel.
 * height: the height of the bitmap in pixel.
 */
void write_bitmap_header(FILE *stream, int width, int height);

#endif // _BITMAP_H
