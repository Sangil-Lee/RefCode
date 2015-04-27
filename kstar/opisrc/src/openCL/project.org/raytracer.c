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
#include "timers.h"
#include <jpeglib.h>

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


typedef struct _Vec3
{
	T x,y,z;
}Vec3;

void print(Vec3* v)
{
	printf("[%f %f %f]\n", v->x, v->y, v->z);
}

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

T length(Vec3* v) 
{ 
	return sqrt(length2(v)); 
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

T dot(Vec3* v1, Vec3* v2) 
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



/*
class Vec3
{
	public:
		T x, y, z;
		Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
		Vec3(T xx) : x(xx), y(xx), z(xx) {}
		Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
		Vec3& normalize()
		{
			T nor2 = length2();
			if (nor2 > 0) {
				T invNor = 1 / sqrt(nor2);
				x *= invNor, y *= invNor, z *= invNor;
			}
			return *this;
		}
		Vec3 operator * (const T &f) const { return Vec3(x * f, y * f, z * f); }
		Vec3 operator * (const Vec3 &v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
		T dot(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }
		Vec3 operator - (const Vec3 &v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
		Vec3 operator + (const Vec3 &v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
		Vec3& operator += (const Vec3 &v) { x += v.x, y += v.y, z += v.z; return *this; }
		Vec3& operator *= (const Vec3 &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
		Vec3 operator - () const { return Vec3(-x, -y, -z); }
		T length2() const { return x * x + y * y + z * z; }
		T length() const { return sqrt(length2()); }
		friend std::ostream & operator << (std::ostream &os, const Vec3 &v)
		{
			os << "[" << v.x << " " << v.y << " " << v.z << "]";
			return os;
		}
};
*/

typedef struct _Sphere
{
	Vec3 center;                         /// position of the sphere
	T radius, radius2;                   /// sphere radius and radius^2
	Vec3 surfaceColor, emissionColor;    /// surface color and emission (light)
	T transparency, reflection;          /// surface transparency and reflectivity
}Sphere;

void Sphere_init(Sphere* s,
		Vec3 *c, const T r, Vec3 *sc,
		const T refl, 
		const T transp, 
		Vec3* ec)  
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

// compute a ray-sphere intersection using the geometric solution
int intersect(Sphere* s, Vec3* rayorig, Vec3* raydir, T *t0, T *t1)
{
	Vec3 l;
	Vec3_minus(&s->center, rayorig, &l);

	T tca = dot(&l, raydir);
	if (tca < 0) 
		return FALSE;

	T d2 = dot(&l, &l) - tca * tca;
	if (d2 > s->radius2) 
		return FALSE;

	T thc = sqrt(s->radius2 - d2);
	if (t0 != NULL && t1 != NULL) {
		*t0 = tca - thc;
		*t1 = tca + thc;
	}

	return TRUE;
}

/*
class Sphere
{
	public:
		Vec3 center;                         /// position of the sphere
		T radius, radius2;                   /// sphere radius and radius^2
		Vec3 surfaceColor, emissionColor;    /// surface color and emission (light)
		T transparency, reflection;          /// surface transparency and reflectivity
		Sphere(const Vec3 &c, const T &r, const Vec3 &sc, 
				const T &refl = 0, const T &transp = 0, const Vec3 &ec = 0) : 
			center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec),
			transparency(transp), reflection(refl)
	{}
		// compute a ray-sphere intersection using the geometric solution
		bool intersect(const Vec3 &rayorig, const Vec3 &raydir, T *t0 = NULL, T *t1 = NULL) const
		{
			Vec3 l = center - rayorig;
			T tca = l.dot(raydir);
			if (tca < 0) return false;
			T d2 = l.dot(l) - tca * tca;
			if (d2 > radius2) return false;
			T thc = sqrt(radius2 - d2);
			if (t0 != NULL && t1 != NULL) {
				*t0 = tca - thc;
				*t1 = tca + thc;
			}

			return true;
		}
};
*/

#define MAX_RAY_DEPTH 5

T mix(const T a, const T b, const T mix)
{
	return b * mix + a * (1 - mix);
}

T max(const T a, const T b)
{
	if(a > b) {
		return a;
	}
	return b;
}

T min(const T a, const T b)
{
	if(a > b) {
		return b;
	}
	return a;
}


// This is the main trace function. It takes a ray as argument (defined by its origin
// and direction). We test if this ray intersects any of the geometry in the scene.
// If the ray intersects an object, we compute the intersection point, the normal
// at the intersection point, and shade this point using this information.
// Shading depends on the surface property (is it transparent, reflective, diffuse).
// The function returns a color for the ray. If the ray intersects an object that
// is the color of the object at the intersection point, otherwise it returns
// the background color.
void trace(Vec3 *rayorig, Vec3 *raydir, 
		Sphere* spheres, int num_spheres,
		const int depth,
		Vec3* ret)
{

	//print(rayorig);
	//print(raydir);


	//if (raydir.length() != 1) std::cerr << "Error " << raydir << std::endl;
	T tnear = INFINITY;
	Sphere *sphere = NULL;
	Vec3 tmp, tmp2, tmp3;
	
	// find intersection of this ray with the sphere in the scene
	for (int i = 0; i < num_spheres; ++i) {
		T t0 = INFINITY, t1 = INFINITY;
		if (intersect(&spheres[i], rayorig, raydir, &t0, &t1)) {
			if (t0 < 0) t0 = t1;
			if (t0 < tnear) {
				tnear = t0;
				sphere = &spheres[i];
			}
		}
	}
	
	// if there's no intersection return black or background color
	if (!sphere) {
		Vec3_init2(ret, 2);
		return;
	}

	// color of the ray/surfaceof the object intersected by the ray
	Vec3 surfaceColor; 
	Vec3_init(&surfaceColor);

	//Vec3 phit = rayorig + raydir * tnear; // point of intersection
	Vec3 phit;
	Vec3_mul(raydir, tnear, &tmp);
	Vec3_plus(rayorig, &tmp, &phit);

	//Vec3 nhit = phit - sphere->center; // normal at the intersection point
	Vec3 nhit;
	Vec3_minus(&phit, &sphere->center, &nhit);
	Vec3_normalize(&nhit); // normalize normal direction

	// If the normal and the view direction are not opposite to each other 
	// reverse the normal direction. That also means we are inside the sphere so set
	// the inside bool to true. Finally reverse the sign of IdotN which we want
	// positive.
	T bias = 1e-4; // add some bias to the point from which we will be tracing
	int inside = FALSE;
	if (dot(raydir, &nhit) > 0) {
		Vec3_inverse(&nhit, &nhit);
		inside = TRUE;
	}

	if ((sphere->transparency > 0 || sphere->reflection > 0) && depth < MAX_RAY_DEPTH) {
		T facingratio = -dot(raydir, &nhit);
		// change the mix value to tweak the effect
		T fresneleffect = mix(pow((T)1 - facingratio, 3), 1, 0.1); 
		// compute reflection direction (not need to normalize because all vectors
		// are already normalized)
		//Vec3 refldir = raydir - nhit * 2 * raydir.dot(nhit);
		Vec3 refldir;
		Vec3_mul(&nhit, 2 * dot(raydir, &nhit), &tmp);
		Vec3_minus(raydir, &tmp, &refldir);
		Vec3_normalize(&refldir);

		Vec3 reflection;
		Vec3_mul(&nhit, bias, &tmp2);
		Vec3_plus(&phit, &tmp2, &tmp);
		
		trace(&tmp, &refldir, spheres, num_spheres, depth + 1, &reflection);

		Vec3 refraction;
		Vec3_init(&refraction);

		// if the sphere is also transparent compute refraction ray (transmission)
		if (sphere->transparency) {
			T ior = 1.1, eta = (inside) ? ior : 1 / ior; // are we inside or outside the surface?
			T cosi = -dot(&nhit, raydir);
			T k = 1 - eta * eta * (1 - cosi * cosi);
			Vec3 refrdir;
			Vec3_mul(raydir, eta, &tmp);
			Vec3_mul(&nhit, eta * cosi - sqrt(k), &tmp2);
			Vec3_plus(&tmp, &tmp2, &refrdir);

			Vec3_normalize(&refrdir);

			Vec3_mul(&nhit, bias, &tmp2);
			Vec3_minus(&phit, &tmp2, &tmp);

			trace(&tmp, &refrdir, spheres, num_spheres, depth + 1, &refraction);
		}
		// the result is a mix of reflection and refraction (if the sphere is transparent)
		Vec3_mul(&reflection, fresneleffect, &tmp);
		Vec3_mul(&refraction, (1 - fresneleffect) * sphere->transparency, &tmp2);
		Vec3_plus(&tmp, &tmp2, &tmp3);

		Vec3_mul2(&tmp3, &sphere->surfaceColor, &surfaceColor);
	}
	else {
		// it's a diffuse object, no need to raytrace any further
		for (int i = 0; i < num_spheres; ++i) {
			if (spheres[i].emissionColor.x > 0) {
				// this is a light
				Vec3 transmission;
				Vec3_init2(&transmission, 1);
				Vec3 lightDirection;
				Vec3_minus(&spheres[i].center, &phit, &lightDirection);
				Vec3_normalize(&lightDirection);

				for (int j = 0; j < num_spheres; ++j) {
					if (i != j) {
						T t0, t1;
						Vec3_mul(&nhit, bias, &tmp);
						Vec3_plus(&phit, &tmp, &tmp2);
						if (intersect(&spheres[j], &tmp2, &lightDirection, &t0, &t1)) {
							Vec3_init(&transmission);
							break;
						}
					}
				}
			
				Vec3_mul2(&sphere->surfaceColor, &transmission, &tmp);
				Vec3_mul(&tmp, max((T)0, dot(&nhit, &lightDirection)), &tmp2);
				Vec3_mul2(&tmp2, &spheres[i].emissionColor, &tmp3);
				Vec3_plus_assign(&surfaceColor, &tmp3);
			}
		}
	}


	Vec3_plus(&surfaceColor, &sphere->emissionColor, ret);
}

typedef struct _stackItem
{
	//Arguments
	Vec3* rayorig;
	Vec3* raydir;
	int depth;
	Vec3* ret;

	//Local Vars
	Vec3 tmp, tmp2, tmp3;
	T tnear;
	Sphere* sphere;
	Vec3 surfaceColor;
	Vec3 phit;
	Vec3 nhit;
	int inside;
	Vec3 refldir;
	Vec3 reflection;
	Vec3 refrdir;
	Vec3 refraction;

	T fresneleffect;
	T bias;

	int step;

}stackItem;

void trace_non_recursion(Vec3 *_rayorig, Vec3 *_raydir, Sphere* spheres, int num_spheres, const int _depth, Vec3* _ret)
{

	int p = 0;
	stackItem stack[MAX_RAY_DEPTH+1]; 
	memset(stack, 0x00, sizeof(stackItem)*MAX_RAY_DEPTH);

	stack[p].rayorig = _rayorig;
	stack[p].raydir = _raydir;
	stack[p].depth = _depth;
	stack[p].ret = _ret;

	stack[p].step = 0;

	while(1) {

		if(stack[p].step == 0) {

			stack[p].tnear = INFINITY;
			stack[p].sphere = NULL;

			// find intersection of this ray with the sphere in the scene
			for (int i = 0; i < num_spheres; ++i) {
				T t0 = INFINITY;
				T t1 = INFINITY;
				if (intersect(&spheres[i], stack[p].rayorig, stack[p].raydir, &t0, &t1)) {
					if (t0 < 0) t0 = t1;
					if (t0 < stack[p].tnear) {
						stack[p].tnear = t0;
						stack[p].sphere = &spheres[i];
					}
				}
			}

			// if there's no intersection return black or background color
			if (!stack[p].sphere) {

				Vec3_init2(stack[p].ret, 2);
				p--;

				if(p < 0) {
					return;
				}
				else {
					continue;
				}
			}

			// color of the ray/surfaceof the object intersected by the ray
			Vec3_init(&stack[p].surfaceColor);

			// point of intersection
			Vec3_mul(stack[p].raydir, stack[p].tnear, &stack[p].tmp);
			Vec3_plus(stack[p].rayorig, &stack[p].tmp, &stack[p].phit);

			//Vec3 nhit = phit - sphere->center; // normal at the intersection point
			Vec3_minus(&stack[p].phit, &stack[p].sphere->center, &stack[p].nhit);
			Vec3_normalize(&stack[p].nhit); // normalize normal direction

			// If the normal and the view direction are not opposite to each other 
			// reverse the normal direction. That also means we are inside the sphere so set
			// the inside bool to true. Finally reverse the sign of IdotN which we want
			// positive.
			stack[p].bias = 1e-4; // add some bias to the point from which we will be tracing
			stack[p].inside = FALSE;
			if (dot(stack[p].raydir, &stack[p].nhit) > 0) {
				Vec3_inverse(&stack[p].nhit, &stack[p].nhit);
				stack[p].inside = TRUE;
			}

			if ((stack[p].sphere->transparency > 0 || stack[p].sphere->reflection > 0) && stack[p].depth < MAX_RAY_DEPTH) {
				T facingratio = -dot(stack[p].raydir, &stack[p].nhit);
				// change the mix value to tweak the effect
				stack[p].fresneleffect = mix(pow((T)1 - facingratio, 3), 1, 0.1); 
				// compute reflection direction (not need to normalize because all vectors
				// are already normalized)
				//Vec3 refldir = raydir - nhit * 2 * raydir.dot(nhit);
				Vec3_mul(&stack[p].nhit, 2 * dot(stack[p].raydir, &stack[p].nhit), &stack[p].tmp);
				Vec3_minus(stack[p].raydir, &stack[p].tmp, &stack[p].refldir);
				Vec3_normalize(&stack[p].refldir);

				Vec3_mul(&stack[p].nhit, stack[p].bias, &stack[p].tmp2);
				Vec3_plus(&stack[p].phit, &stack[p].tmp2, &stack[p].tmp);

				//trace_non_recursion(&tmp, &refldir, spheres, num_spheres, depth + 1, &stack[p].reflection);
#ifdef DEBUG
				printf("[%d] call trace at step1\n", p);
				fflush(stdout);
#endif
				{
					stack[p].step = 1;

					stack[p+1].step = 0;
					stack[p+1].rayorig = &stack[p].tmp;
					stack[p+1].raydir = &stack[p].refldir;
					stack[p+1].depth = stack[p].depth + 1;
					stack[p+1].ret = &stack[p].reflection;
					p++;

					continue;
				}
			}
			else {
				// it's a diffuse object, no need to raytrace any further
				for (int i = 0; i < num_spheres; ++i) {
					if (spheres[i].emissionColor.x > 0) {
						// this is a light
						Vec3 transmission;
						Vec3_init2(&transmission, 1);
						Vec3 lightDirection;
						Vec3_minus(&spheres[i].center, &stack[p].phit, &lightDirection);
						Vec3_normalize(&lightDirection);

						for (int j = 0; j < num_spheres; ++j) {
							if (i != j) {
								T t0, t1;
								Vec3_mul(&stack[p].nhit, stack[p].bias, &stack[p].tmp);
								Vec3_plus(&stack[p].phit, &stack[p].tmp, &stack[p].tmp2);
								if (intersect(&spheres[j], &stack[p].tmp2, &lightDirection, &t0, &t1)) {
									Vec3_init(&transmission);
									break;
								}
							}
						}

						Vec3_mul2(&stack[p].sphere->surfaceColor, &transmission, &stack[p].tmp);
						Vec3_mul(&stack[p].tmp, max((T)0, dot(&stack[p].nhit, &lightDirection)), &stack[p].tmp2);
						Vec3_mul2(&stack[p].tmp2, &spheres[i].emissionColor, &stack[p].tmp3);
						Vec3_plus_assign(&stack[p].surfaceColor, &stack[p].tmp3);
					}
				}
			}
		}
		else if(stack[p].step == 1) {
#ifdef DEBUG
			printf("[%d] resume trace == step1\n", p);
			fflush(stdout);
#endif

			stack[p].step = 2;

			Vec3_init(&stack[p].refraction);

			// if the sphere is also transparent compute refraction ray (transmission)
			if (stack[p].sphere->transparency) {
				T ior = 1.1, eta = (stack[p].inside) ? ior : 1 / ior; // are we inside or outside the surface?
				T cosi = -dot(&stack[p].nhit, stack[p].raydir);
				T k = 1 - eta * eta * (1 - cosi * cosi);
				Vec3_mul(stack[p].raydir, eta, &stack[p].tmp);
				Vec3_mul(&stack[p].nhit, eta * cosi - sqrt(k), &stack[p].tmp2);
				Vec3_plus(&stack[p].tmp, &stack[p].tmp2, &stack[p].refrdir);

				Vec3_normalize(&stack[p].refrdir);

				Vec3_mul(&stack[p].nhit, stack[p].bias, &stack[p].tmp2);
				Vec3_minus(&stack[p].phit, &stack[p].tmp2, &stack[p].tmp);

				//trace_non_recursion(&tmp, &refrdir, spheres, num_spheres, depth + 1, &refraction);
#ifdef DEBUG
				printf("[%d] call trace at step2\n", p);
				fflush(stdout);
#endif

				{

					stack[p+1].step = 0;
					stack[p+1].rayorig = &stack[p].tmp;
					stack[p+1].raydir = &stack[p].refrdir;
					stack[p+1].depth = stack[p].depth + 1;
					stack[p+1].ret = &stack[p].refraction;
					p++;

				}
			}
			continue;
		}
		else if(stack[p].step == 2) {

#ifdef DEBUG
			printf("[%d] resume trace == step2\n", p);
			fflush(stdout);
#endif

			// the result is a mix of reflection and refraction (if the sphere is transparent)
			Vec3_mul(&stack[p].reflection, stack[p].fresneleffect, &stack[p].tmp);
			Vec3_mul(&stack[p].refraction, (1 - stack[p].fresneleffect) * stack[p].sphere->transparency, &stack[p].tmp2);
			Vec3_plus(&stack[p].tmp, &stack[p].tmp2, &stack[p].tmp3);

			Vec3_mul2(&stack[p].tmp3, &stack[p].sphere->surfaceColor, &stack[p].surfaceColor);
		}

		Vec3_plus(&stack[p].surfaceColor, &stack[p].sphere->emissionColor, stack[p].ret);


#ifdef DEBUG
		printf("return trace %d\n",p);
				fflush(stdout);
#endif
		p--;
		if(p < 0) {
			return;
		}
	}
}


void save_jpeg_image(const char* filename, Vec3* image, int image_width, int image_height);
// Main rendering function. We compute a camera ray for each pixel of the image
// trace it and return a color. If the ray hits a sphere, we return the color of the
// sphere at the intersection point, else we return the background color.
void render(Sphere* spheres, int num_spheres, char* filename, int width, int height)
{
	Vec3 *image = (Vec3*)malloc(sizeof(Vec3) * width * height); 
	Vec3 *pixel = image;

	T invWidth = 1 / (T)width, invHeight = 1 / (T)height;
	T fov = 30, aspectratio = width / (T)height;
	T angle = tan(M_PI * 0.5 * fov / (T)180);

	timer_start(3);

	// +leesi. Here, parallel code.
	// Trace rays
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x, ++pixel) {
			T xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			T yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			Vec3 raydir;
			Vec3_assign(&raydir, xx, yy, -1);
			Vec3_normalize(&raydir);
			Vec3 tmp;
			Vec3_init(&tmp);
			trace_non_recursion(&tmp, &raydir, spheres, num_spheres, 0, pixel);
		}
	}
	timer_stop(3);

	timer_start(4);
	// Save result to a jpg image
	save_jpeg_image(filename, image, width, height);
	timer_stop(4);
/*
	// Save result to a PPM image
	FILE *fp;
	if((fp = fopen("result.ppm", "wb")) == NULL) {
		fprintf(stderr, "Error.. cannot open the result.ppm file");
		exit(1);
	}
	fprintf(fp, "P6\n%d %d\n255\n", width, height);
	for (int i = 0; i < width * height; ++i) {
		unsigned char c[3];
		c[0] = (unsigned char)(min((T)1, image[i].x) * 255);
		c[1] = (unsigned char)(min((T)1, image[i].y) * 255);
		c[2] = (unsigned char)(min((T)1, image[i].z) * 255);

		fwrite(c, 3, 1, fp);
	}
	fclose(fp);
*/
	/*
	std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for (unsigned i = 0; i < width * height; ++i) {
		ofs << (unsigned char)(std::min(T(1), image[i].x) * 255) << 
			(unsigned char)(std::min(T(1), image[i].y) * 255) <<
			(unsigned char)(std::min(T(1), image[i].z) * 255); 
	}
	ofs.close();
	*/
	free(image);

}

int main(int argc, char **argv)
{
	char filename[1024];
	int width = 1024;
	int height = 640;

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
	}

	timer_start(1);

	Sphere* spheres;

	spheres = (Sphere*)malloc(sizeof(Sphere) * 6);

	// position, radius, surface color, reflectivity, transparency, emission color
	//sphere0
	{
		Vec3 c, sc, ec;
		
		Vec3_assign(&c, 0, -10004, -20);
		Vec3_assign(&sc, 0.2, 0.2, 0.2);
		Vec3_assign(&ec, 0.0, 0.0, 0.0);

		Sphere_init(&spheres[0], &c, 10000, &sc, 0, 0.0, &ec);
	}

	//sphere1
	{
		Vec3 c, sc, ec;
		
		Vec3_assign(&c, 0, 0, -20);
		Vec3_assign(&sc, 1.00, 0.32, 0.36);
		Vec3_assign(&ec, 0.0, 0.0, 0.0);

		Sphere_init(&spheres[1], &c, 4, &sc, 1, 0.5, &ec);
	}

	//sphere2
	{
		Vec3 c, sc, ec;
		
		Vec3_assign(&c, 5, -1, -15);
		Vec3_assign(&sc, 0.90, 0.76, 0.46);
		Vec3_assign(&ec, 0.0, 0.0, 0.0);

		Sphere_init(&spheres[2], &c, 2, &sc, 1, 0.0, &ec);
	}

	//sphere3
	{
		Vec3 c, sc, ec;
		
		Vec3_assign(&c, 5, 0, -25);
		Vec3_assign(&sc, 0.65, 0.77, 0.97);
		Vec3_assign(&ec, 0.0, 0.0, 0.0);

		Sphere_init(&spheres[3], &c, 3, &sc, 1, 0.0, &ec);
	}

	//sphere4
	{
		Vec3 c, sc, ec;
		
		Vec3_assign(&c, -5.5, 0, -15);
		Vec3_assign(&sc, 0.90, 0.90, 0.90);
		Vec3_assign(&ec, 0.0, 0.0, 0.0);

		Sphere_init(&spheres[4], &c, 3, &sc, 1, 0.0, &ec);
	}

	//sphere5
	{
		Vec3 c, sc, ec;
		
		Vec3_assign(&c, 0, 20, -30);
		Vec3_assign(&sc, 0.0, 0.0, 0.0);
		Vec3_assign(&ec, 3.0, 3.0, 3.0);

		Sphere_init(&spheres[5], &c, 3, &sc, 0, 0.0, &ec);
	}
	timer_stop(1);
/*
	spheres.push_back(new Sphere(Vec3(0, 0, -20), 4, Vec3(1.00, 0.32, 0.36), 1, 0.5)); //1
	spheres.push_back(new Sphere(Vec3(5, -1, -15), 2, Vec3(0.90, 0.76, 0.46), 1, 0.0));//2
	spheres.push_back(new Sphere(Vec3(5, 0, -25), 3, Vec3(0.65, 0.77, 0.97), 1, 0.0));//3
	spheres.push_back(new Sphere(Vec3(-5.5, 0, -15), 3, Vec3(0.90, 0.90, 0.90), 1, 0.0));//4
	// light
	spheres.push_back(new Sphere(Vec3(0, 20, -30), 3, Vec3(0), 0, 0, Vec3(3)));//5
*/
	timer_start(2);
	render(spheres, 6, filename, width, height);
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
}
