#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <jpeglib.h>
#include "timers.h"

#define T float

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#ifndef INFINITY
#define INFINITY 1e8
#endif

#define MAX_RAY_DEPTH 5


typedef struct _RGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGB;

void save_jpeg_image(const char* filename, Vec3* image, int image_width, int image_height)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer;

	RGB* rgb = (RGB*)malloc(sizeof(RGB)*image_width*image_height);
	for (int i = 0; i < image_width * image_height; ++i) {
		rgb[i].r = (unsigned char)(min((T)1, image[i].x) * 255);
		rgb[i].g = (unsigned char)(min((T)1, image[i].y) * 255);
		rgb[i].b = (unsigned char)(min((T)1, image[i].z) * 255);
	}

	int i;
	FILE* fp;

	cinfo.err = jpeg_std_error(&jerr);

	fp = fopen(filename, "wb");
	if( fp == NULL )
	{
		printf("Cannot open file to save jpeg image: %s\n", filename);
		exit(0);
	}
	
	jpeg_create_compress(&cinfo);

	jpeg_stdio_dest(&cinfo, fp);

	cinfo.image_width = image_width;
	cinfo.image_height = image_height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);

	jpeg_start_compress(&cinfo, TRUE);

	for(i = 0; i < image_height; i++ )
	{
		row_pointer = (JSAMPROW)&rgb[i*image_width];
		jpeg_write_scanlines(&cinfo, &row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(fp);
};
