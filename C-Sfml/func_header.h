//raytracer function header : Elric Christopher 2019

#include "def_header.h"

//---random type---
unsigned int GenRand()
{
	unsigned int randVal;
	_rdrand32_step(&randVal);
	return randVal;
}
//-

//----color type ----
Color color_make(double r, double g, double b) {
	Color c;
	c.r = MIN(r, 1.0);
	c.g = MIN(g, 1.0);
	c.b = MIN(b, 1.0);
	return c;
}
Color color_makeFromRGB(unsigned char r, unsigned char g, unsigned char b) {
	return color_make(r / 255.0, g / 255.0, b / 255.0);
}
Color color_makeFromRGBhex(unsigned int c) {
	return color_makeFromRGB(c >> 16, c >> 8, c);
}
//-

//------vector3 type------
static double invSqrt_1(double y) {
	double xhalf = (double)0.5 * y;
	long long i = *(long long*)(&y);
	i = 0x5fe6ec85e7de30daLL - (i >> 1); //LL suffix for (long long) type for VS
	y = *(double*)(&i);
	y = y * ((double)1.5 - xhalf * y * y);
	return y;
}
Vector3 vec3_make(double x, double y, double z) {
	Vector3 v;
	v.x = x, v.y = y, v.z = z;
	return v;
}
Vector3 vec3_spherical_to_cartesian(Vector3 data) //x = phi, y = theta , r = y
{
	return (Vector3) { .x = data.z*sin(data.y)*cos(data.x), .y = data.z* cos(data.y), .z = -data.z*sin(data.y)*sin(data.x) };
}
double vec3_length(Vector3 v) {
	return sqrt(SQUARE(v.x) + SQUARE(v.y) + SQUARE(v.z));
}
Vector3 vec3_cartesian_to_spherical(Vector3 data)
{
	return (Vector3) { .x = acos(data.y / vec3_length(data)), .y = atan(-data.z / data.x), .z = vec3_length(data) };
}
Vector3 vec3_mult(Vector3 v, double b) {
	return vec3_make(v.x * b, v.y * b, v.z * b);
}
Vector3 vec3_unit(Vector3 v) {
	double is = invSqrt_1(SQUARE(v.x) + SQUARE(v.y) + SQUARE(v.z));
	return vec3_mult(v, is);
}
Vector3 vec3_sub(Vector3 a, Vector3 b) {
	return vec3_make(a.x - b.x, a.y - b.y, a.z - b.z);
}
Vector3 vec3_add(Vector3 a, Vector3 b) {
	return vec3_make(a.x + b.x, a.y + b.y, a.z + b.z);
}
Vector3 vec3_cross(Vector3 a, Vector3 b) {
	Vector3 v;
	v.x = (a.y * b.z) - (b.y * a.z);
	v.y = (a.z * b.x) - (b.z * a.x);
	v.z = (a.x * b.y) - (b.x * a.y);
	return v;
}
//--

//-----Material type------
Material material_make(Color color, double reflectivity, double reflectionNoise, double specularity) //color-reflec
{
	Material m;
	m.color = color;
	m.reflectivity = reflectivity;
	m.specularity = specularity;
	m.reflectionNoise = reflectionNoise;
	return m;
}
//--

//-------Geometry type-----
Sphere sphere_make(Vector3 center, double radius) {
	Sphere s = { .center = center, .radius = radius };
	return s;
}

Triangle triangle_make(Vector3 a, Vector3 b, Vector3 c) {
	Triangle t = { .a = a,.b = b,.c = c };
	VEC3_SUB(t.edges[0], t.b, t.a);
	VEC3_SUB(t.edges[1], t.c, t.a);
	t.normal = vec3_unit(vec3_cross(vec3_sub(t.b, t.a), vec3_sub(t.c, t.a)));
	return t;
}
//--

//-------Surface type-----
Surface surface_initSphere(Vector3 position, double radius, Material material) {
	Surface s = { .type = GTSphere,
				  .geometry.sphere = sphere_make(position, radius),
				  .material = material };
	return s;
}
Surface surface_initTriangle(Vector3 a, Vector3 b, Vector3 c, Material material) {
	Surface t = { .type = GTTriangle,
				  .geometry.triangle = triangle_make(a, b, c),
				  .material = material };
	return t;
}
//--

//-------Light type------
Light light_make(Vector3 position, double intensity) {
	Light l = { .position = position,.intensity = intensity };
	return l;
}
//--

//--------Array type------
Array *array_init(Array *a, unsigned int itemSize, unsigned int initialCapacity) {
	if (initialCapacity == 0) {
		initialCapacity = 1; // Idiot-proof
	}
	a->items = malloc(itemSize * initialCapacity);
	if (a->items == NULL) {
		free(a);
		return NULL;
	}
	a->itemSize = itemSize;
	a->capacity = initialCapacity;
	a->count = 0;
	return a;
}
void array_dealloc(Array *a) {
	free(a->items);
}
int array_add(Array *a, void *item) { //add item to Array
	void *reallocPtr;
	unsigned int newCapacity, memOffset;
	if (a->count >= a->capacity) {
		newCapacity = a->capacity * 2;
		reallocPtr = realloc(a->items, newCapacity * a->itemSize);
		if (reallocPtr == NULL) {
			return 0;
		}
		a->items = reallocPtr;
		a->capacity = newCapacity;
	}
	memOffset = (a->count * a->itemSize);
	memmove(a->items + memOffset, item, a->itemSize);
	a->count++;
	return 1;
}
void* array_get(Array *a, unsigned int index) { //by index
	return a->items + (index * a->itemSize);
}
int array_addArray(Array *array, Array *toAdd) {
	unsigned int i;
	for (i = 0; i < toAdd->count; i++) {
		if (!array_add(array, array_get(toAdd, i))) {
			return 0;
		}
	}
	return 1;
}
void array_remove(Array *a, unsigned int index) { //by index
	void *itemToRemove, *itemToMove;
	if (index >= a->count) {
		return;
	}
	if (a->count == 1) {
		a->count--;
		return;
	}
	itemToRemove = a->items + (index * a->itemSize);
	itemToMove = a->items + (a->count * a->itemSize);
	memmove(itemToRemove, itemToMove, a->itemSize);
	a->count--;
}
//--

//------Camera type---- Lásd: http://people.cs.clemson.edu/~dhouse/courses/405/notes/raycamera.pdf
void camera_init(Camera *cam, Vector3 position, Vector3 direction, double focalLength, double width, double height) {
	cam->position = position;
	cam->direction = vec3_unit(direction);
	cam->focalLength = focalLength;
	cam->width = width;
	cam->height = height;
	//-
	Vector3 dir = vec3_cartesian_to_spherical(direction);
	cam->angle.x = dir.x;
	cam->angle.y = dir.y;
}

void camera_setup(Camera *c) {
	Vector3 x_c = c->position;
	Vector3 u_c = c->direction;
	double z_p = c->focalLength;
	Vector3 v_up = vec3_make(0.0, -1.0, 0.0);
	Vector3 c_0 = vec3_add(x_c, vec3_mult(u_c, z_p));
	Vector3 u_x = vec3_unit(vec3_cross(u_c, v_up));
	Vector3 u_y = vec3_cross(vec3_mult(u_c, -1.0), u_x);
	c->planeCenter = c_0;
	c->planeDirectionX = u_x;
	c->planeDirectionY = u_y;
	//    Vector3 u_z = vec3_mult(u_c, -1.0); // Normal to the view plane
}
//--

//------scene type----
void scene_init(Scene *scene, size_t width, size_t height) {
	array_init(&scene->surfaces, INIT_CAPACITY, sizeof(Surface));
	array_init(&scene->lights, INIT_CAPACITY, sizeof(Light));
	camera_init(&scene->camera, VEC3_ZERO, vec3_make(0, 0, 1), 430.0, width, height);
	scene->ambientCoefficient = 0.6;
	scene->backgroundColor = COLOR_WHITE;
}
void scene_dealloc(Scene *scene) {
	array_dealloc(&scene->surfaces);
	array_dealloc(&scene->lights);
}
void scene_AddLight(Scene *scene, Light *light) {
	array_add(&scene->lights, light);
}
void scene_AddLightRange(Scene *scene, Light *lights, size_t len) {
	for (size_t i = 0; i < len; i++) {
		scene_AddLight(scene, &lights[i]);
	}
}
void scene_addSurface(Scene *scene, Surface *surface) {
	array_add(&scene->surfaces, surface);
}
void scene_addSurfaceRange(Scene *scene, Surface *surfaces, size_t len) {
	for (size_t i = 0; i < len; i++) {
		scene_addSurface(scene, &surfaces[i]);
	}
}

//-mesh inside scene

typedef struct 
{
	float Normal[3]; //32 bit floating point
	float Vertex1[3]; //32 bit floating point
	float Vertex2[3]; //32 bit floating point
	float Vertex3[3]; //32 bit floating point
	unsigned short Attr; //2 bytes
}STL_Triangle;

Array mesh_load(char* filename, Material material) { //filename expected to be stl
	Array triangles;
	FILE* fp = fopen(filename, "rb"); //read - binary
	if (fp == NULL)goto fp_error;
	array_init(&triangles, sizeof(Surface), 1500); //triangles witch size is 3xVector3
	fseek(fp, 80, 0);
	size_t NumOfTriangles; //32 bit unsigned int
	fread(&NumOfTriangles, sizeof(size_t), 1, fp);
	STL_Triangle* Data = malloc(NumOfTriangles * sizeof(STL_Triangle));
	if (Data == NULL)goto fp_error;
	
	//fread(Data, sizeof(STL_Triangle), NumOfTriangles-1, fp); //hát ez durva...mi a sz...hibás adatok?
	for (size_t i = 0; i < NumOfTriangles; i++)
	{
		fread(Data[i].Normal, sizeof(float), 3, fp);
		fread(Data[i].Vertex3, sizeof(float), 3, fp);
		fread(Data[i].Vertex2, sizeof(float), 3, fp);
		fread(Data[i].Vertex1, sizeof(float), 3, fp);
		fread(&Data[i].Attr, 2, 1, fp);
	}
	Surface tr;
	for (size_t i = 0; i < NumOfTriangles; i++)
	{
		tr = surface_initTriangle(vec3_make(Data[i].Vertex1[0], Data[i].Vertex1[1], Data[i].Vertex1[2]), vec3_make(Data[i].Vertex2[0], Data[i].Vertex2[1], Data[i].Vertex2[2]), vec3_make(Data[i].Vertex3[0], Data[i].Vertex3[1], Data[i].Vertex3[2]), material);
		array_add(&triangles, &tr);
	}
	fclose(fp);
	printf("Triangles: %d\n", triangles.count);
	return triangles;
fp_error:
	array_init(&triangles, sizeof(Surface), 1);
	return triangles;

}
//-

void scene_loadMesh(Scene* scene, char* filename, Material meshMaterial)
{
	Array mesh = mesh_load(filename, meshMaterial);
	array_addArray(&scene->surfaces, &mesh);
}
#include "scene_lands.h"
//--


//------raytracer type----
void raytracer_init(Raytracer *rt, size_t resolutionX, size_t resolutionY, long NumOfProcs) {
	scene_init(&rt->scene, resolutionX, resolutionY);
	rt->resolutionX = resolutionX;
	rt->resolutionY = resolutionY;
	rt->_Threading._nprocs = (size_t)NumOfProcs;
}
void raytracer_dealloc(Raytracer *rt) {
	scene_dealloc(&rt->scene);
	free(rt->_Threading._ray_threading);
	free(rt->_Threading._threads);
}
#include "raytracer_parallel.h"
void raytracer_render_start(Raytracer *rt, void *data) {
	camera_setup(&rt->scene.camera);
	rt->_Threading._ray_threading = (Ray_Threading **)malloc(sizeof(Ray_Threading **) * rt->_Threading._nprocs);
	rt->_Threading._threads = (sfThread **)malloc(sizeof(sfThread **) * rt->_Threading._nprocs);
	for (int i = 0; i < rt->_Threading._nprocs; i++) //elõkészítés
	{
		rt->_Threading._ray_threading[i] = malloc(sizeof(Ray_Threading));
		rt->_Threading._ray_threading[i]->_continue = malloc(sizeof(bool));
		*rt->_Threading._ray_threading[i]->_continue = true;
		rt->_Threading._ray_threading[i]->Th_Div = ((rt->resolutionX* rt->resolutionY) / rt->_Threading._nprocs) * i;
		rt->_Threading._ray_threading[i]->data = data;
		rt->_Threading._ray_threading[i]->rt = rt;
		rt->_Threading._ray_threading[i]->hwrnd = rt->hw_random;
		rt->_Threading._threads[i] = sfThread_create(ray_parallel, rt->_Threading._ray_threading[i]);
		sfThread_launch(rt->_Threading._threads[i]);
	}
	return;
}
//--


//------render type-----
void set_camera_angle(Vector3 *cm, Vector2 *angle)
{
	*cm = vec3_spherical_to_cartesian((Vector3) { angle->x, angle->y, vec3_length(*cm) });
}
void set_hwrnd(Ray_XYRandom* rnd,Raytracer* rt)
{
	for (size_t y = 0; y < rt->resolutionY; y++)
	{
		for (size_t x = 0; x < rt->resolutionX; x++)
		{
			rnd[(y* rt->resolutionX) + x].y = y;
			rnd[(y* rt->resolutionX) + x].x = x;
		}
	}
}
#if shuffle_type == 1
void pick_hwrnd(Ray_XYRandom *from, unsigned int _peek, Ray_XYRandom *to, unsigned int _poke)
{
	to[_poke].x = from[_peek].x;
	to[_poke].y = from[_peek].y;
	//-
	for (unsigned int i = _peek; i <= _poke; i++)
	{
		from[i].x = from[i + 1].x;
		from[i].y = from[i + 1].y;
	}
}
#else
void shf_hwrnd(Ray_XYRandom* rnd ,Raytracer* rt) //rnd
{
	for (size_t y = 0; y < rt->resolutionY; y++)
	{
		for (size_t x = 0; x < rt->resolutionX; x++)
		{
#if random_type == 0
			size_t w_srnd = randnum(0, rt->resolutionX -1), h_srnd = randnum(0, rt->resolutionY -1),
				s_y = rnd[(y* rt->resolutionX) + x].y, s_x = rnd[(y* rt->resolutionX) + x].x;
#else
			size_t w_srnd = s_rand(0, rt->resolutionX - 1), h_srnd = s_rand(0, rt->resolutionY - 1),
				s_y = rnd[(y * rt->resolutionX) + x].y, s_x = rnd[(y * rt->resolutionX) + x].x;
#endif	
			rnd[(y*rt->resolutionX) + x].y = rnd[(h_srnd* rt->resolutionX) + w_srnd].y;
			rnd[(y*rt->resolutionX) + x].x = rnd[(h_srnd* rt->resolutionX) + w_srnd].x;
			rnd[(h_srnd*rt->resolutionX) + w_srnd].x = s_x;
			rnd[(h_srnd* rt->resolutionX) + w_srnd].y = s_y;
		}
	}
}
#endif
void shuffle_hwrnd(Raytracer *rt, unsigned int _size)
{
#if shuffle_type == 1
	Ray_XYRandom *tmp = malloc(sizeof(Ray_XYRandom) * _size + 1);
	memset(tmp, 0, sizeof(Ray_XYRandom) * _size + 1);
	memset(rnd, 0, sizeof(Ray_XYRandom) * _size);
	set_hwrnd(tmp, rt);
	for (unsigned int i = _size - 1; i > 0; i--)
	{
		pick_hwrnd(tmp, randnum(0, i), rt->hw_random, i);
	}
	pick_hwrnd(tmp, 0, rnd, 0);
	free(tmp);
#else
	memset(rt->hw_random, 0, sizeof(Ray_XYRandom) * _size);
	set_hwrnd(rt->hw_random, rt);
	shf_hwrnd(rt->hw_random, rt);
#endif
}
#include "csfml_graph_render.h"
//--

//--- main selector type --
void select_Land(int argc, const char *argv[], Scene *scene) {
	if (argc != 2) {
		char stage[30];
		printf("Parancssor hasznalat: %s [helyszin]\n", argv[0]);
		printf("Elerheto helyszinek: 'box', 'flat'. For External: 'ex'\r\n");
		printf("Helyszin(string):");
		scanf("%s", stage);
		if (STREQ(stage, "box"))
		{
			scene_loadBoxLand(scene);
			return;
		}
		else if (STREQ(stage, "ex"))
		{
			printf("\r\nSTL file path:");
			char fname[30];
			scanf("%s", fname);
			scene_loadExternalLand(scene, fname);
			return;
		}
		else if(!STREQ(stage, "flat"))
		printf("\r\nHelyszinnek csak a 'flat' vagy a 'box' szot fogadja el a program.\r\n Alapertelmezett(flat) betoltese...\r\n");
		scene_loadflatLand(scene); //alapért.
	}
	else if (STREQ(argv[1], "box")) {
		scene_loadBoxLand(scene);
	}
	else if (STREQ(argv[1], "flat")) {
		scene_loadflatLand(scene);
	}
	else
	{
		printf("\r\nHelyszinnek csak a 'flat' vagy a 'box' szot fogadja el a program.\r\n Alapertelmezett(flat) betoltese...\r\n");
		scene_loadflatLand(scene); //alapért.
	}
}

//--
