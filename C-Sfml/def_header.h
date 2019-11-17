//raytracer definitions header : Elric Christopher
//

#define WINDOW_WIDTH 800 //todo: make this changeable
#define WINDOW_HEIGHT 600 //todo: make this changeable

#define MAX_RECURSION_DEPTH 2 //egyelore max: 2
#define MAX_VISIBLE_DISTANCE 1500.0

#define threads 4 //amount of render threads (not used anymore)
#define EPSILON 0.000001

#define INIT_CAPACITY 256 //scene

// Specular reflection constant
#define KS_CONST 1.0

//number gen type
#define shuffle_type 0 //1=slow, 0=faster
#define random_type 1 //1=psuedo 0=real

//--------------------macros------------------------
//-----utils
#define SQUARE(x) ((x)*(x))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LIMIT(val, min, max) ((val) > (max) ? (max) : ((val) < (min) ? (min) : (val)))
#define STREQ(a,b) (strcmp(a,b) == 0)
//-----utils-end

//----Vector
#define VEC3_ZERO vec3_make(0.0, 0.0, 0.0)

#define VEC3_ADD(out, a, b)    \
            (out).x = (a).x + (b).x; \
            (out).y = (a).y + (b).y; \
            (out).z = (a).z + (b).z;

#define VEC3_ADD3(out, a, b, c)      \
            (out).x = (a).x + (b).x + (c).x; \
            (out).y = (a).y + (b).y + (c).x; \
            (out).z = (a).z + (b).z + (c).x;

#define VEC3_SUB(out, a, b)    \
            (out).x = (a).x - (b).x; \
            (out).y = (a).y - (b).y; \
            (out).z = (a).z - (b).z;

#define VEC3_DOT(a, b) ((a).x * (b).x + (a).y * (b).y + (a).z * (b).z)

#define VEC3_CROSS(out, a, b)          \
            (out).x=((a).y * (b).z) - ((b).y * (a).z); \
            (out).y=((a).z * (b).x) - ((b).z * (a).x); \
            (out).z=((a).x * (b).y) - ((b).x * (a).y);

#define VEC3_MULT(out, a, b) \
            (out).x = (a).x * b; \
            (out).y = (a).y * b; \
            (out).z = (a).z * b

#define VEC3_DIV(out, a, b)  \
            (out).x = (a).x / b; \
            (out).y = (a).y / b; \
            (out).z = (a).z / b; 

#define VEC3_MULTBYVEC3(out, a, b) \
            (out).x = (a).x * (b).x;     \
            (out).y = (a).y * (b).y;     \
            (out).z = (a).z * (b).z;
//-----Vector-end


//----Colors
#define COLOR_BLACK color_makeFromRGBhex(0x000000)
#define COLOR_WHITE color_makeFromRGBhex(0xFFFFFF)
#define COLOR_RED   color_makeFromRGBhex(0xFF0000)
#define COLOR_GREEN color_makeFromRGBhex(0x00FF00)
#define COLOR_BLUE  color_makeFromRGBhex(0x0000FF)

#define COLORS1_RED    color_makeFromRGB(189, 42, 51)
#define COLORS1_YELLOW color_makeFromRGB(214, 170, 38)
#define COLORS1_GREEN  color_makeFromRGB(64, 129, 86)
#define COLORS1_GREEN2 color_makeFromRGB(147, 163, 28)
#define COLORS1_BLUE   color_makeFromRGB(48, 55, 79)

#define COLORS2_RED    color_makeFromRGBhex(0xC00F12)
#define COLORS2_ORANGE color_makeFromRGBhex(0xD27701)
#define COLORS2_YELLOW  color_makeFromRGBhex(0xDDBA01)
#define COLORS2_GREEN color_makeFromRGBhex(0x007A28)
#define COLORS2_BLUE   color_makeFromRGBhex(0x3B9B95)
//-----Colors -end


//-------------------------------Macros end.---------------------------------


//------Enums-----
enum GeometryType {
	GTSphere,
	GTTriangle
};

typedef enum bool{
	false = 0,
	true = 1
}bool;
//--------Enums-end------

//-------Structs & enums----

typedef struct {
	unsigned int x, y;
}Ray_XYRandom;

typedef struct {
	double x, y, z;
} Vector3;

typedef struct
{
	double x, y;
} Vector2;

typedef struct {
	double r, g, b;
} Color;

typedef struct {
	unsigned char *items; //Az elem Array kezdõ címe.
	unsigned int itemSize; //Egy elem mérete.
	unsigned int count; //Hány darab hozzáadott elem
	unsigned int capacity; //Hány elem fér ebbe bele. (Több is hozzá adható, ilyenkor duplázódik a mérete.)
} Array;

typedef struct {
	double diffused;
	double specular;
} ShadingResult;

typedef struct {
	Vector3 center;
	double radius;
} Sphere;

typedef struct {
	Vector3 a, b, c;
	Vector3 edges[2];
	Vector3 normal;
} Triangle;

union Geometry {
	Sphere sphere;
	Triangle triangle;
};

typedef struct {
	Vector3 position;
	Vector3 direction;
	Vector3 planeCenter;
	Vector3 planeDirectionX;
	Vector3 planeDirectionY;
	double focalLength;
	double width;
	double height;
	Vector2 angle;
} Camera;

typedef struct {
	Color color;
	double reflectivity;
	double specularity;
	double reflectionNoise;
} Material;

typedef struct {
	enum GeometryType type;
	union Geometry geometry;
	Material material;
} Surface;

typedef struct {
	Vector3 position;
	double intensity;
} Light;

typedef struct {
	Array faces;
	Sphere boundingVolume;
} Mesh;

typedef struct {
	Vector3 origin;
	Vector3 direction;
} Ray;

typedef struct {
	Surface *surface;
	double distance;
} TracingResult;

typedef struct {
	Array surfaces;
	Array lights;
	double ambientCoefficient;
	Camera camera;
	Color backgroundColor;
} Scene;

struct Ray_Threading;
struct Raytracer {
	Scene scene;
	size_t resolutionX;
	size_t resolutionY;
	struct
	{
		size_t _nprocs; //number of available processor cores.
		sfThread **_threads; //threads that will work on rendering.
		struct Ray_Threading **_ray_threading;
	}_Threading;
	Ray_XYRandom *hw_random;
};

struct Ray_Threading {
	int Th_Div;
	struct Raytracer *rt;
	void *data;
	Ray_XYRandom *hwrnd;
	bool *_continue;
};

typedef struct Raytracer Raytracer;
typedef struct Ray_Threading Ray_Threading;

enum FileStatus {
	FSHeader,
	FSVertices,
	FSFaces
};

//-keys
typedef struct
{
	struct 
	{
		bool KeyW, KeyA, KeyS, KeyD;
	}walking;
	struct 
	{
		bool KeySpace, KeyLShift;
	}special;
	struct
	{
		bool KeyE, KeyQ, KeyR, KeyF;
	}others;
	struct
	{
		bool Up, Down, Left, Right;
	}Arrows;
	struct 
	{
		bool n0, n1, n2, n3, n4, n5;
	}numeric;
} rt_keyboard;
//-

//------------Structs-end

//---------Func_Macros --------------------
//---random type---
#define randnum(_min, _max) \
        ((GenRand() % (int)(((_max) + 1) - (_min))) + (_min))
#define s_rand(_min, _max) \
        ((rand() % (int)(((_max) + 1) - (_min))) + (_min))
//-

//----color type ----
#define _color_addWeighted(cout, _ca, _weightA, _cb, _weightB) { \
	double sum = weightA + weightB; \
	cout.r = ((_ca.r * _weightA) + (_cb.r * _weightB)) / sum; \
	cout.g = ((_ca.g * _weightA) + (_cb.g * _weightB)) / sum; \
	cout.b = ((_ca.b * _weightA) + (_cb.b * _weightB)) / sum; }

#define _color_mult(out, n) { \
	out.r = MIN(out.r*(n), 1.0); \
	out.g = MIN(out.g*(n), 1.0); \
	out.b = MIN(out.b*(n), 1.0);}

#define _color_blend(out, _Ca, _weightA, _Cb) { \
	double weightA = LIMIT(_weightA, 0.0, 1.0); \
	double weightB = 1.0 - _weightA; \
	_color_addWeighted(out, _Ca, _weightA, _Cb, weightB); }
//-

//------vector3 type------
#define _invSqrt_1(y) { double xhalf = (double)0.5 * y; long long i = *(long long*)(&y); \
		i = 0x5fe6ec85e7de30daLL - (i >> 1); y = *(double*)(&i); y = y * ((double)1.5 - xhalf * y * y); }
#define _vec3_length(out, v) { out = sqrt(SQUARE(v.x) + SQUARE(v.y) + SQUARE(v.z)); }
#define _vec3_unit(out, v) { double is = SQUARE(v.x) + SQUARE(v.y) + SQUARE(v.z); _invSqrt_1(is); VEC3_MULT(out, v, is); }
//-

//-------Geometry type-----
//-

//-------Surface type-----
#define _surface_getNormalAtPoint(out, _surface, _point) { \
	switch (_surface->type) { \
	case GTTriangle: out = _surface->geometry.triangle.normal; break; \
	case GTSphere: { Vector3 v3; VEC3_SUB(v3, _surface->geometry.sphere.center, _point); _vec3_unit(out, v3); } break; \
	default: out = (Vector3) { 0,0,0 }; break; }}
//-

//-------Light type------
#define _light_getDirection(out, light_position, point) { \
		Vector3 v3; VEC3_SUB(v3, point, light_position); \
		double is = SQUARE(v3.x) + SQUARE(v3.y) + SQUARE(v3.z); _invSqrt_1(is); \
		VEC3_MULT(out, v3, is); }

#define _light_getDiffusedHighlight(out, light_intensity, direction, normal) { \
	double highlight = VEC3_DOT(normal, direction); \
	out = (highlight < 0) ? 0 : highlight * light_intensity; }

#define _light_getSpecularHighlight(out, light_intensity, lightDirection, normal, rayDirection, specularity) { \
double highlight = VEC3_DOT(normal, lightDirection); Vector3 V = (Vector3) { -rayDirection.x, -rayDirection.y, -rayDirection.z }; \
Vector3 R; VEC3_MULT(R, normal, highlight * 2.0); VEC3_SUB(R,lightDirection, R); \
double dot = VEC3_DOT(V, R); out = (dot < 0) ? 0 : pow(dot, specularity) * KS_CONST * light_intensity; }
//-

//---Array type----
#define ARRAY_GET(a, index) ((void*)((a)->items + ((index) * (a)->itemSize)))
//-

//------Camera type----
//-

//------scene type----
//-

//------raytracer type----
#define _ray_makeForPixel(out, camera, _x, _y) {  \
		double py = (-(camera).height / 2.0) + 1.0 * ((double)(_y) + 0.5); \
		double px = (-(camera).width / 2.0) + 1.0 * ((double)(_x) + 0.5); \
		Vector3 a = (camera).planeCenter; \
		Vector3 b = { (camera).planeDirectionX.x*px, (camera).planeDirectionX.y*px, (camera).planeDirectionX.z*px }; \
		Vector3 c = { (camera).planeDirectionY.x*py, (camera).planeDirectionY.y*py, (camera).planeDirectionY.z*py }; \
		Vector3 p = { a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z }; \
		Vector3 v = { p.x - (camera).position.x, p.y - (camera).position.y, p.z - (camera).position.z }; \
		double is = SQUARE(v.x) + SQUARE(v.y) + SQUARE(v.z); \
		_invSqrt_1(is); \
		(out) = (Ray){ .origin = (camera).position, .direction = { v.x * is, v.y * is, v.z * is } }; }

#define _ray_traceOnce(out, ray, scene) { \
	(out) = (TracingResult){ .surface = NULL, .distance = DBL_MAX }; \
	double distance = DBL_MAX; int hit = 0; \
	for (size_t i = 0; i < scene.surfaces.count; i++) { \
	Surface *surface = ARRAY_GET(&scene.surfaces, i); \
	switch (surface->type) { \
	case GTSphere: _ray_checkSphereIntersection(hit, ray, surface->geometry.sphere, distance); break; \
	case GTTriangle: _ray_checkTriangleIntersection(hit, ray, surface->geometry.triangle, distance);break; \
	default: hit = 0; break; } \
	if (hit && distance < (out).distance && distance > EPSILON) {(out).distance = distance; (out).surface = surface;}}}

#define _ray_addNoise(out, _epsilon) { \
		double r = (((double)rand() / RAND_MAX) * 2 * _epsilon) - _epsilon; \
		out.direction.x += r; \
		r = (((double)rand() / RAND_MAX) * 2 * _epsilon) - _epsilon; \
		out.direction.y += r; \
		r = (((double)rand() / RAND_MAX) * 2 * _epsilon) - _epsilon; \
		out.direction.z += r;}

#define _ray_shadeAtPoint(out, ray, scene, _surface, point) { \
	Light *light; double lightDistance; Vector3 lightDirection; Ray newRay; TracingResult shadowTracingResult; \
	out.diffused = 0.0; out.specular = 0.0; newRay.origin = point; \
	for (unsigned int i = 0; i < scene.lights.count; i++) { \
	light = ARRAY_GET(&scene.lights, i); \
	_light_getDirection(lightDirection, light->position, point); \
	newRay.direction = (Vector3) { -lightDirection.x, -lightDirection.y, -lightDirection.z }; \
	Vector3 v; VEC3_SUB(v, light->position, point); \
	_vec3_length(lightDistance, v); \
	_ray_traceOnce(shadowTracingResult, newRay, scene); \
	if (shadowTracingResult.surface == NULL || shadowTracingResult.distance > lightDistance) { \
		Vector3 normal; _surface_getNormalAtPoint(normal, _surface, point); \
		double dl; _light_getDiffusedHighlight(dl, light->intensity, lightDirection, normal); \
		out.diffused += dl; \
		_light_getSpecularHighlight(dl, light->intensity, lightDirection, normal, ray.direction, _surface->material.specularity); \
		out.specular += dl; }}}

#define _ray_reflect(out, _ray, _surface, _point) { \
	Vector3 surfaceNormal; _surface_getNormalAtPoint(surfaceNormal,_surface, _point); \
	double c1 = -VEC3_DOT(surfaceNormal, _ray.direction); \
	Vector3 M; VEC3_MULT(M, surfaceNormal, (2 * c1)); \
	Vector3 RI; VEC3_ADD(RI,_ray.direction, M); \
	out.origin = _point; out.direction = RI;}

// http://stackoverflow.com/questions/14074643/why-does-raytracer-render-spheres-as-ovals
#define _ray_checkSphereIntersection(out, ray, sphere, distance) { \
			double a = VEC3_DOT(ray.direction, ray.direction); \
			Vector3 ro_sc; VEC3_SUB(ro_sc, ray.origin, sphere.center); \
			double b = 2.0 * VEC3_DOT(ray.direction, ro_sc); \
			double c = VEC3_DOT(ro_sc, ro_sc) - SQUARE(sphere.radius); \
			double disc = SQUARE(b) - 4 * a * c; \
			if(disc < 0){ (out) = 0; } else { double distSqrt = sqrt(disc); \
			double q = b < 0.0 ? (-b - distSqrt) / 2.0 : (-b + distSqrt) / 2.0; \
			double t0 = q / a; double t1 = c / q; if (t0 > t1) { double swap = t0; t0 = t1; t1 = swap; } \
			if (t1 < 0) { (out) = 0;}else if (t0 < 0) {distance = t1; (out) = 1; }else {distance = t0; (out) = 1;} } }

// http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf
#define _ray_checkTriangleIntersection(out, ray, triangle, distance) { \
			Vector3 pvec, tvec, qvec; VEC3_CROSS(pvec, ray.direction, triangle.edges[0]); \
			double det = VEC3_DOT(triangle.edges[1], pvec); \
			if (det < EPSILON) { (out) = 0; } else { VEC3_SUB(tvec, ray.origin, triangle.a); double u = VEC3_DOT(tvec, pvec); \
			if ((u < 0.0) || (u > det)) { (out) = 0; } else { VEC3_CROSS(qvec, tvec, triangle.edges[1]); double v = VEC3_DOT(ray.direction, qvec); \
			if ((v < 0.0) || ((u + v) > det)) { (out) = 0; } else { double d = VEC3_DOT(triangle.edges[0], qvec);double inv_det = 1.0 / det; \
			d *= inv_det; distance = d; (out) = 1; } } } }

#define _getHighlightedColor(out, _highlight, _ambientCoef) { \
		double n = _ambientCoef + _highlight.diffused * (1.0 - _ambientCoef); \
		out.r = MIN((out.r*n) + _highlight.specular, 1.0); \
		out.g = MIN((out.g*n) + _highlight.specular, 1.0); \
		out.b = MIN((out.b*n) + _highlight.specular, 1.0); }
//-

//---------Func_Macros-end.--------------------
