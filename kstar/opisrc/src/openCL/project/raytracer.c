/*
	 A very basic raytracer example.
	 Copyright (C) 2012  www.scratchapixel.com

	 This program is free software: you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation, either version 3 of the License, or
	 (at your option) any later version.

	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 GNU General Public License for more details.

	 You should have received a copy of the GNU General Public License
	 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <strings.h>
#include <jpeglib.h>
#include "timers.h"
#include <CL/cl.h>
#include "cl_util.h"

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
#define DEBUG 1

static size_t WIDTH = 6400;
static size_t HEIGHT = 3200;
static size_t SPHERECNT = 6;

typedef struct _Vec3
{
	T x,y,z;
}Vec3;

typedef struct _Sphere
{
	Vec3 center;                         /// position of the sphere
	T radius, radius2;                   /// sphere radius and radius^2
	Vec3 surfaceColor, emissionColor;    /// surface color and emission (light)
	T transparency, reflection;          /// surface transparency and reflectivity
}Sphere;

static Vec3 *M_A  = NULL;

typedef struct _RGB
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGB;

void Vec3_init(Vec3* v)
{
	v->x = 0;
	v->y = 0;
	v->z = 0;
}

void Vec3_init2(Vec3* v, T xx)
{
	v->x = xx;
	v->y = xx;
	v->z = xx;
}

void Vec3_assign(Vec3* v, T xx, T yy, T zz)
{
	v->x = xx;
	v->y = yy;
	v->z = zz;
}

T length2(Vec3* v) 
{ 
	return v->x * v->x + v->y * v->y + v->z * v->z; 
}

void Vec3_normalize(Vec3* v)
{
	T nor2 = length2(v);
	if(nor2 > 0) {
		T invNor = 1 / sqrt(nor2);
		v->x *= invNor, v->y *= invNor, v->z *= invNor;
	}
}

void Vec3_mul(Vec3* v, const T f, Vec3* ret) 
{
	Vec3_assign(ret, v->x * f, v->y * f, v->z * f);
}

void Vec3_mul2(Vec3* v1, Vec3* v2, Vec3* ret) 
{ 
	Vec3_assign(ret, v1->x * v2->x, v1->y * v2->y, v1->z * v2->z); 
}

T dotting(Vec3* v1, Vec3* v2) 
{ 
	return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z; 
}

void Vec3_minus(Vec3* v1, Vec3* v2, Vec3* ret) 
{ 
	Vec3_assign(ret, v1->x - v2->x, v1->y - v2->y, v1->z - v2->z); 
}

void Vec3_plus(Vec3* v1, Vec3* v2, Vec3* ret)  
{ 
	Vec3_assign(ret, v1->x + v2->x, v1->y + v2->y, v1->z + v2->z); 
}

void Vec3_plus_assign(Vec3* v1, Vec3* v2) 
{ 
	v1->x += v2->x, v1->y += v2->y, v1->z += v2->z;  
}

void Vec3_minus_assign(Vec3* v1, Vec3* v2) 
{ 
	v1->x -= v2->x, v1->y -= v2->y, v1->z -= v2->z;  
}

void Vec3_mul_assign(Vec3* v1, Vec3* v2) 
{ 
	v1->x *= v2->x, v1->y *= v2->y, v1->z *= v2->z;
}

void Vec3_inverse(Vec3* v, Vec3* ret) 
{ 
	Vec3_assign(ret, -v->x, -v->y, -v->z); 
}

void Sphere_init(Sphere* s, Vec3 *c, const T r, Vec3 *sc, const T refl, 
		const T transp, Vec3* ec)  
{
	Vec3_assign(&s->center, c->x, c->y, c->z);
	s->radius = r;
	s->radius2 = r * r;
	Vec3_assign(&s->surfaceColor, sc->x, sc->y, sc->z);
	if(ec != 0) {
		Vec3_assign(&s->emissionColor, ec->x, ec->y, ec->z);
	}
	else {
		Vec3_init(&s->emissionColor);
	}

	s->transparency = transp; 
	s->reflection = refl;
}
T min(const T a, const T b)
{
	if(a > b) {
		return b;
	}
	return a;
}
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

// Main rendering function. We compute a camera ray for each pixel of the image
// trace it and return a color. If the ray hits a sphere, we return the color of the
// sphere at the intersection point, else we return the background color.
void render(Sphere* spheres, int num_spheres, char* filename, int width, int height)
{
	//opencl
	cl_platform_id   *platform;
	cl_device_type   dev_type = CL_DEVICE_TYPE_DEFAULT;
	cl_device_id     dev;
	cl_context       context;
	cl_command_queue cmd_queue;
	cl_program       program;
	cl_kernel        kernel;
	cl_mem           mem_A;
	cl_mem           mem_Sphere;
	cl_int           err;
	cl_uint          num_platforms;
	cl_uint          num_dev = 0;

	//+opencl
	char *dtype = getenv("CL_DEV_TYPE");
	if (dtype) {
		if (strcasecmp(dtype, "cpu") == 0) {
			dev_type = CL_DEVICE_TYPE_CPU;
		} else if (strcasecmp(dtype, "gpu") == 0) {
			dev_type = CL_DEVICE_TYPE_GPU;
		}
	}

	//Platforms
	err = clGetPlatformIDs(0, NULL, &num_platforms);
#if DEBUG 
	CHECK_ERROR(err);
	if (num_platforms == 0) {
		fprintf(stderr, "[%s:%d] ERROR: No OpenCL platform\n", __FILE__,__LINE__);
		exit(EXIT_FAILURE);
	}
	printf("Number of platforms: %u\n", num_platforms);
#endif
	platform = (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);
	err = clGetPlatformIDs(num_platforms, platform, NULL);
#if DEBUG 
	CHECK_ERROR(err);
#endif
	//Device
	for (int i = 0; i < num_platforms; i++) {
		err = clGetDeviceIDs(platform[i], dev_type, 1, &dev, &num_dev);
#if DEBUG
		if (err != CL_DEVICE_NOT_FOUND) CHECK_ERROR(err);
#endif
		if (num_dev == 1) break;
	}
	if (num_dev < 1) {
		fprintf(stderr, "[%s:%d] ERROR: No device\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	// Context
	context = clCreateContext(NULL, 1, &dev, NULL, NULL, &err);
	CHECK_ERROR(err);

	// Command queue
	cmd_queue = clCreateCommandQueue(context, dev, 0, &err);
	CHECK_ERROR(err);

	// Create a program.
	size_t source_len;
	char *source_code = get_source_code("./trace_non.cl", &source_len);
	program = clCreateProgramWithSource(context, 1, (const char **)&source_code, &source_len, &err);
	//free(source_code);
	CHECK_ERROR(err);

	// Build the program.
	char build_opts[200];
	sprintf(build_opts, "-DWIDTH=%lu -DHEIGHT=%lu -DSPHERECNT=%lu", WIDTH, HEIGHT, SPHERECNT);
	err = clBuildProgram(program, 1, &dev, build_opts, NULL, NULL);

	if (err == CL_SUCCESS) 
	{
		print_build_log(program, dev);
		CHECK_ERROR(err);

		// Kernel
		kernel = clCreateKernel(program, "trace_ray", &err);
		CHECK_ERROR(err);

		//Buffers
		mem_A = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, 
				sizeof(Vec3) * HEIGHT * WIDTH, M_A, &err);
		CHECK_ERROR(err);

	    mem_Sphere = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 
				sizeof(Sphere)*SPHERECNT, spheres, &err);
		CHECK_ERROR(err);

		// Set the arguments.
		err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mem_A);
		CHECK_ERROR(err);

		err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &mem_Sphere);
		CHECK_ERROR(err);

		// Enqueue the kernel.
		size_t lws[2] = {16, 16};
		size_t gws[2];
		gws[1] = (size_t)ceil((double)WIDTH / lws[0]) * lws[0];
		gws[0] = (size_t)ceil((double)HEIGHT / lws[1]) * lws[1];
		printf("H:%lu, W:%lu\n", gws[0], gws[1]);

		err = clEnqueueNDRangeKernel(cmd_queue,
				kernel, 2, NULL, gws, lws, 0, NULL, NULL);
		CHECK_ERROR(err);

		//-opencl
		//Vec3 *image = (Vec3*)malloc(sizeof(Vec3) * width * height); 
		//Vec3 *pixel = image;

		timer_start(3);

#if 0
		T invWidth = 1 / (T)width; 
		T invHeight = 1 / (T)height;
		T fov = 30;
		T aspectratio = width / (T)height;
		T angle = tan(M_PI * 0.5 * fov / (T)180);

		// +leesi. Here, parallel code.
		// Trace rays
		for (int y = 0; y < height; ++y) 
		{
			for (int x = 0; x < width; ++x, ++pixel) 
			{
				T xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
				T yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
				trace_non_recursion(xx, yy, spheres, num_spheres, 0, pixel);
			};
		};
#endif
		timer_stop(3);

		// Read the result.
		err = clEnqueueReadBuffer(cmd_queue, mem_A,
				CL_TRUE, 0, sizeof(Vec3) * HEIGHT * WIDTH, M_A, 0, NULL, NULL);
		CHECK_ERROR(err);

		timer_start(4);
		
		// Save result to a jpg image
		Vec3 *image = (Vec3*)M_A;
		save_jpeg_image(filename, image, width, height);
		timer_stop(4);

		// Release
		clReleaseMemObject(mem_A);
		clReleaseKernel(kernel);
		clReleaseProgram(program);
		clReleaseCommandQueue(cmd_queue);
		clReleaseContext(context);
		free(platform);
		//free(image);
	}
}

void Make_speres(Sphere *spheres, const int count)
{
	{
	Vec3 c, sc, ec;
	//sphere0
	Vec3_assign(&c, 0, -10004, -20);
	Vec3_assign(&sc, 0.2, 0.2, 0.2);
	Vec3_assign(&ec, 0.0, 0.0, 0.0);
	Sphere_init(&spheres[0], &c, 10000, &sc, 0, 0.0, &ec);
	}

	{
	//sphere1
	Vec3 c, sc, ec;
	Vec3_assign(&c, 0, 0, -20);
	Vec3_assign(&sc, 1.00, 0.32, 0.36);
	Vec3_assign(&ec, 0.0, 0.0, 0.0);
	Sphere_init(&spheres[1], &c, 4, &sc, 1, 0.5, &ec);
	}

	{
	//sphere2
	Vec3 c, sc, ec;
	Vec3_assign(&c, 5, -1, -15);
	Vec3_assign(&sc, 0.90, 0.76, 0.46);
	Vec3_assign(&ec, 0.0, 0.0, 0.0);
	Sphere_init(&spheres[2], &c, 2, &sc, 1, 0.0, &ec);
	}

	{
	//sphere3
	Vec3 c, sc, ec;
	Vec3_assign(&c, 5, 0, -25);
	Vec3_assign(&sc, 0.65, 0.77, 0.97);
	Vec3_assign(&ec, 0.0, 0.0, 0.0);
	Sphere_init(&spheres[3], &c, 3, &sc, 1, 0.0, &ec);
	}

	{
	//sphere4
	Vec3 c, sc, ec;
	Vec3_assign(&c, -5.5, 0, -15);
	Vec3_assign(&sc, 0.90, 0.90, 0.90);
	Vec3_assign(&ec, 0.0, 0.0, 0.0);
	Sphere_init(&spheres[4], &c, 3, &sc, 1, 0.0, &ec);
	}

	{
	//sphere5
	Vec3 c, sc, ec;
	Vec3_assign(&c, 0, 20, -30);
	Vec3_assign(&sc, 0.0, 0.0, 0.0);
	Vec3_assign(&ec, 3.0, 3.0, 3.0);
	Sphere_init(&spheres[5], &c, 3, &sc, 0, 0.0, &ec);
	}
}

int main(int argc, char **argv)
{
	char filename[1024];
	int width = 6400;
	int height = 3200;

	timer_init();
	timer_start(0);

	if(argc < 2) {
		sprintf(filename, "%s", "result.jpg");	
	}
	else if(argc == 2) {
		sprintf(filename, "%s", argv[1]);	
	}
	else if(argc == 3) {
		fprintf(stderr, "$> raytracer <filename> <width> <height>\n");
	}
	else if(argc == 4) {
		sprintf(filename, "%s", argv[1]);	
		width = atoi(argv[2]);
		height = atoi(argv[3]);
	};

	HEIGHT = height;
	WIDTH = width;

	M_A  = (Vec3 *)malloc(sizeof(Vec3)*HEIGHT*WIDTH);

	Sphere* spheres;
	spheres = (Sphere*)malloc(sizeof(Sphere) * SPHERECNT);

	timer_start(1);
	// position, radius, surface color, reflectivity, transparency, emission color
	Make_speres(spheres, SPHERECNT);
	timer_stop(1);

	timer_start(2);
	render(spheres, SPHERECNT, filename, width, height);
	timer_stop(2);

	free(spheres);
	timer_stop(0);
	
	printf("Raytracing: w=%d, h=%d, %s\n", width, height, filename);
	printf("Init time : %f sec\n", timer_read(1));
	printf("Render time : %f sec\n", timer_read(2));
	printf("Total time : %f sec\n", timer_read(0));
	printf("Trace time : %f sec\n", timer_read(3));
	printf("IO time : %f sec\n", timer_read(4));
	printf("\n");

	return 0;
}
