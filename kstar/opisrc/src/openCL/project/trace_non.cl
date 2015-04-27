#define T float
#define NULL 0

#define TRUE 1
#define FALSE 0
#define MAX_RAY_DEPTH 5

typedef struct _Vec3
{
	T x,y,z;
}Vec3;

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

typedef struct _Sphere
{
	Vec3 center;                         /// position of the sphere
	T radius, radius2;                   /// sphere radius and radius^2
	Vec3 surfaceColor, emissionColor;    /// surface color and emission (light)
	T transparency, reflection;          /// surface transparency and reflectivity
}Sphere;

int intersect(Sphere* s, Vec3* rayorig, Vec3* raydir, T *t0, T *t1)
{
	Vec3 l;
	Vec3_minus(&s->center, rayorig, &l);

	T tca = dotting(&l, raydir);
	if (tca < 0) 
		return FALSE;

	T d2 = dotting(&l, &l) - tca * tca;
	if (d2 > s->radius2) 
		return FALSE;

	T thc = sqrt(s->radius2 - d2);
	if (t0 != NULL && t1 != NULL) {
		*t0 = tca - thc;
		*t1 = tca + thc;
	}

	return TRUE;
}

#if 1
T mixing(const T a, const T b, const T m)
{
	return b * m + a * (1 - m);
}
#endif

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
} stackItem;

void trace_non_recursion(float xx, float yy, Sphere* spheres, int num_spheres, Vec3* _ret)
{
	int p = 0;
	stackItem stack[MAX_RAY_DEPTH+1]; 
	//memset(stack, 0x00, sizeof(stackItem)*MAX_RAY_DEPTH);

	Vec3 tmp;
	Vec3_init(&tmp);
	Vec3 raydir;
	Vec3_assign(&raydir, xx, yy, -1);
	Vec3_normalize(&raydir);

	stack[p].rayorig = &tmp;
	stack[p].raydir = &raydir;
	stack[p].ret = _ret;
	stack[p].depth = 0;

	stack[p].step = 0;

	while(1) 
	{
		if(stack[p].step == 0) 
		{
			stack[p].tnear = INFINITY;
			stack[p].sphere = NULL;

			// find intersection of this ray with the sphere in the scene
			for (int i = 0; i < num_spheres; ++i) 
			{
				T t0 = INFINITY;
				T t1 = INFINITY;
				if (intersect(&spheres[i], stack[p].rayorig, stack[p].raydir, &t0, &t1)) 
				{
					if (t0 < 0) t0 = t1;
					if (t0 < stack[p].tnear) 
					{
						stack[p].tnear = t0;
						stack[p].sphere = &spheres[i];
					};
				};
			};

			// if there's no intersection return black or background color
			if (!stack[p].sphere) 
			{
				Vec3_init2(stack[p].ret, 2);
				p--;

				if(p < 0) return;
				else continue;
			};

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

			if (dotting(stack[p].raydir, &stack[p].nhit) > 0) 
			{
				Vec3_inverse(&stack[p].nhit, &stack[p].nhit);
				stack[p].inside = TRUE;
			};

			if ((stack[p].sphere->transparency > 0 || stack[p].sphere->reflection > 0) && stack[p].depth < MAX_RAY_DEPTH) 
			{
				T facingratio = -dotting(stack[p].raydir, &stack[p].nhit);
				// change the mixing value to tweak the effect
				stack[p].fresneleffect = mixing(pow((T)1 - facingratio, 3), 1, 0.1); 
				// compute reflection direction (not need to normalize because all vectors
				// are already normalized)
				//Vec3 refldir = raydir - nhit * 2 * raydir.dot(nhit);
				Vec3_mul(&stack[p].nhit, 2 * dotting(stack[p].raydir, &stack[p].nhit), &stack[p].tmp);
				Vec3_minus(stack[p].raydir, &stack[p].tmp, &stack[p].refldir);
				Vec3_normalize(&stack[p].refldir);

				Vec3_mul(&stack[p].nhit, stack[p].bias, &stack[p].tmp2);
				Vec3_plus(&stack[p].phit, &stack[p].tmp2, &stack[p].tmp);

				{
					stack[p].step = 1;
					stack[p+1].step = 0;
					stack[p+1].rayorig = &stack[p].tmp;
					stack[p+1].raydir = &stack[p].refldir;
					stack[p+1].depth = stack[p].depth + 1;
					stack[p+1].ret = &stack[p].reflection;
					p++;
					continue;
				};
			}
			else 
			{
				// it's a diffuse object, no need to raytrace any further
				for (int i = 0; i < num_spheres; ++i) 
				{
					if (spheres[i].emissionColor.x > 0) 
					{
						// this is a light
						Vec3 transmission;
						Vec3_init2(&transmission, 1);
						Vec3 lightDirection;
						Vec3_minus(&spheres[i].center, &stack[p].phit, &lightDirection);
						Vec3_normalize(&lightDirection);

						for (int j = 0; j < num_spheres; ++j) 
						{
							if (i != j) 
							{
								T t0, t1;
								Vec3_mul(&stack[p].nhit, stack[p].bias, &stack[p].tmp);
								Vec3_plus(&stack[p].phit, &stack[p].tmp, &stack[p].tmp2);
								if (intersect(&spheres[j], &stack[p].tmp2, &lightDirection, &t0, &t1)) 
								{
									Vec3_init(&transmission);
									break;
								};
							};
						};
						Vec3_mul2(&stack[p].sphere->surfaceColor, &transmission, &stack[p].tmp);
						Vec3_mul(&stack[p].tmp, max((T)0, dotting(&stack[p].nhit, &lightDirection)), &stack[p].tmp2);
						Vec3_mul2(&stack[p].tmp2, &spheres[i].emissionColor, &stack[p].tmp3);
						Vec3_plus_assign(&stack[p].surfaceColor, &stack[p].tmp3);
					};
				};
			};
		}
		else if(stack[p].step == 1) 
		{
			stack[p].step = 2;
			Vec3_init(&stack[p].refraction);

			// if the sphere is also transparent compute refraction ray (transmission)
			if (stack[p].sphere->transparency) {
				T ior = 1.1, eta = (stack[p].inside) ? ior : 1 / ior; // are we inside or outside the surface?
				T cosi = - dotting(&stack[p].nhit, stack[p].raydir);
				T k = 1 - eta * eta * (1 - cosi * cosi);
				Vec3_mul(stack[p].raydir, eta, &stack[p].tmp);
				Vec3_mul(&stack[p].nhit, eta * cosi - sqrt(k), &stack[p].tmp2);
				Vec3_plus(&stack[p].tmp, &stack[p].tmp2, &stack[p].refrdir);

				Vec3_normalize(&stack[p].refrdir);

				Vec3_mul(&stack[p].nhit, stack[p].bias, &stack[p].tmp2);
				Vec3_minus(&stack[p].phit, &stack[p].tmp2, &stack[p].tmp);

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
		else if(stack[p].step == 2) 
		{

			// the result is a mix of reflection and refraction (if the sphere is transparent)
			Vec3_mul(&stack[p].reflection, stack[p].fresneleffect, &stack[p].tmp);
			Vec3_mul(&stack[p].refraction, (1 - stack[p].fresneleffect) * stack[p].sphere->transparency, &stack[p].tmp2);
			Vec3_plus(&stack[p].tmp, &stack[p].tmp2, &stack[p].tmp3);
			Vec3_mul2(&stack[p].tmp3, &stack[p].sphere->surfaceColor, &stack[p].surfaceColor);
		};

		Vec3_plus(&stack[p].surfaceColor, &stack[p].sphere->emissionColor, stack[p].ret);
		p--;
		if(p < 0) return;
	};
}

__kernel void trace_ray(__global Vec3 *pix, __global Sphere *sphs)
{
	int gx = get_global_id(1);
	int gy = get_global_id(0);

	//printf("x=%d, y=%d\n", gx, gy);
	int height = HEIGHT;
	int width = WIDTH;
	int spherecnt = SPHERECNT;
	//printf("H:%d, W:%d, SPHERECNT=%d\n", height, width, SPHERECNT);

	Sphere s[SPHERECNT];
	int i;
	for(i=0; i < SPHERECNT;i++)
	{
	  s[i].center.x = sphs[i].center.x;
	  s[i].center.y = sphs[i].center.y;
	  s[i].center.z = sphs[i].center.z;
	  s[i].radius = sphs[i].radius;
	  s[i].radius2 = sphs[i].radius2;
	  s[i].surfaceColor.x = sphs[i].surfaceColor.x;
	  s[i].surfaceColor.y = sphs[i].surfaceColor.y;
	  s[i].surfaceColor.z = sphs[i].surfaceColor.z;
	  s[i].emissionColor.x = sphs[i].emissionColor.x;
	  s[i].emissionColor.y = sphs[i].emissionColor.y;
	  s[i].emissionColor.z = sphs[i].emissionColor.z;
	  s[i].transparency = sphs[i].transparency;
	  s[i].reflection = sphs[i].reflection;
	};


	T invWidth = 1 / (T)width;
	T invHeight = 1 / (T)height;
	T fov = 30;
	T aspectratio = width / (T)height;
	T angle = tan(M_PI * 0.5 * fov / (T)180);

	T xx = (2 * ((gx + 0.5) * invWidth) - 1) * angle * aspectratio;
	T yy = (1 - 2 * ((gy + 0.5) * invHeight)) * angle;

	Vec3 pixel;
    trace_non_recursion(xx, yy, s, spherecnt, &pixel);

	pix[gy * width + gx].x = pixel.x;
	pix[gy * width + gx].y = pixel.y;
	pix[gy * width + gx].z = pixel.z;

	//barrier(CLK_GLOBAL_MEM_FENCE);
}

