// raytracer lands
// Néhány összeállítás
// Az alábbiak tartalmat rendelnek a virtuális térbe.
// prog: Elric Christopher

void scene_loadBoxLand(Scene *scene) {
	Surface spheres[6];
	Surface triangles[12];
	Vector3 vs[12];
	Light lights[4];
	scene->ambientCoefficient = 0.5;
	camera_init(&scene->camera, vec3_make(0, 35, -70), vec3_make(0, 0, 1), 500.0,
		scene->camera.width, scene->camera.height);
	lights[0] = light_make(vec3_make(260, 80, -270), 1.0);
	lights[1] = light_make(vec3_make(-30, 65, -300), 0.3);
	spheres[1] = surface_initSphere(vec3_make(45, -20, 70), 20, material_make(COLOR_BLACK, 0.5, 0.05, 20.2));
	spheres[2] = surface_initSphere(vec3_make(-35, -25, 80), 15, material_make(COLOR_BLUE, 0.5, 0.02, 100));
	spheres[0] = surface_initSphere(vec3_make(10, -10, 110), 30, material_make(COLOR_RED, 0.5, 0.01, 40.0));
	spheres[3] = surface_initSphere(vec3_make(10, 40, 110), 25, material_make(COLOR_BLUE, 0.5, 0, 40));
	spheres[4] = surface_initSphere(vec3_make(10, 80, 110), 20, material_make(COLOR_GREEN, 0.5, 0, 40));
	Material sideWallMaterial1 = material_make(COLOR_WHITE, 0.0, 0.0, 40);
	Material sideWallMaterial2 = material_make(COLOR_WHITE, 0.0, 0.0, 40);
	Material ceilingMaterial = material_make(COLOR_WHITE, 0.4, 0.15, 40); //(COLOR_WHITE, 0.4, 0.15, 40)
	Material floorMaterial = material_make(COLOR_WHITE, 0.4, 0.15, 40); //(COLOR_WHITE, 0.4, 0.15, 40)
	vs[0] = vec3_make(-75, -40, 0);
	vs[1] = vec3_make(-75, -40, 150);
	vs[2] = vec3_make(75, -40, 0);
	vs[3] = vec3_make(75, -40, 150);
	vs[4] = vec3_make(-75, 110, 0);
	vs[5] = vec3_make(-75, 110, 150);
	vs[6] = vec3_make(75, 110, 0);
	vs[7] = vec3_make(75, 110, 150);
	vs[8] = vec3_make(-75, -40, -700);
	vs[9] = vec3_make(75, -40, -700);
	vs[10] = vec3_make(75, 110, -700);
	vs[11] = vec3_make(-75, 110, -700);
	//Floor
	triangles[0] = surface_initTriangle(vs[2], vs[1], vs[0], floorMaterial);
	triangles[1] = surface_initTriangle(vs[2], vs[3], vs[1], floorMaterial);
	//Left wall
	triangles[2] = surface_initTriangle((Vector3) { -75, -40, 0 }, (Vector3) { -75, -40, 150 }, (Vector3) { -75, 110, 0 }, sideWallMaterial1);
	triangles[3] = surface_initTriangle(vs[1], vs[5], vs[4], sideWallMaterial1);
	//Right wall
	triangles[4] = surface_initTriangle(vs[6], vs[3], vs[2], sideWallMaterial1);
	triangles[5] = surface_initTriangle(vs[6], vs[7], vs[3], sideWallMaterial1);
	//Ceiling
	triangles[6] = surface_initTriangle(vs[4], vs[5], vs[6], ceilingMaterial);
	triangles[7] = surface_initTriangle(vs[5], vs[7], vs[6], ceilingMaterial);
	//Back
	triangles[8] = surface_initTriangle(vs[3], vs[7], vs[5], sideWallMaterial2);
	triangles[9] = surface_initTriangle(vs[5], vs[1], vs[3], sideWallMaterial2);
	triangles[10] = surface_initTriangle(vs[8], vs[9], vs[10], sideWallMaterial1);
	triangles[11] = surface_initTriangle(vs[8], vs[10], vs[11], sideWallMaterial1);
	scene_addSurfaceRange(scene, spheres, 5); //5
	scene_addSurfaceRange(scene, triangles, 12);
	scene_AddLightRange(scene, lights, 2); //2
}

void scene_loadflatLand(Scene *scene)
{
	srand((size_t)time(NULL)); //reset by time
	scene->ambientCoefficient = 0.5;
	scene->backgroundColor = color_makeFromRGB(153, 217, 234);
	camera_init(&scene->camera, vec3_make(0, 35, -70), vec3_make(0, 0, 1), 500.0,
		scene->camera.width, scene->camera.height);
	//-light
	Light light = light_make(vec3_make(20, 70, -60), 0.6);
	//-
	//Floor
	Material floorMaterial = material_make(color_makeFromRGB(34, 177, 76), 0, 0.1, 40);
	Surface floort[2] = { 
		surface_initTriangle(vec3_make(-300,0,-300), vec3_make(300,0,-300), vec3_make(-300,0,300),floorMaterial),
		surface_initTriangle(vec3_make(300,0,-300), vec3_make(300,0,300), vec3_make(-300,0,300),floorMaterial)
	};
	//-
	// Sky
	Material skymaterial = material_make(color_makeFromRGB(0, 128, 255), 0.2, 0.0, 40);
	Surface skys[2] = {
		surface_initTriangle(vec3_make(300,150,-300), vec3_make(-300,150,-300), vec3_make(-300,150,300),skymaterial),
		surface_initTriangle(vec3_make(300,150,300), vec3_make(300,150,-300), vec3_make(-300,150,300),skymaterial)
	};
	//-
	
	//- Sphere Objects
	Material objects[40];
	for (size_t i = 0; i < 40; i++)
	{
		objects[i] = material_make(color_makeFromRGB(s_rand(0, 254), s_rand(0, 254), s_rand(0, 254)), (double)s_rand(1, 40) / 100.f, 0, (double)s_rand(10, 100));
	}
	Surface spheres[18];
	for (size_t i = 0; i < 18; i++)
	{
		spheres[i] = surface_initSphere(vec3_make(s_rand(-200, 200), (double)s_rand(40, 200) / 10.f, s_rand(-200, 200)), (double)s_rand(20, 90) / 10.f, objects[s_rand(0, 39)]);
	}
	//-
	scene_addSurfaceRange(scene, spheres, 18);
	scene_addSurfaceRange(scene, floort, 2);
	scene_addSurfaceRange(scene, skys, 2);
	scene_AddLight(scene, &light);
}

void scene_loadExternalLand(Scene* scene, char *filename) //csak mesh betöltése. + háttér
{
	Material ExternalMaterial = material_make(COLOR_WHITE, 0.3, 0, 20);
	scene_loadMesh(scene, filename, ExternalMaterial);
	scene->backgroundColor = COLORS1_BLUE;
	Light light = light_make(vec3_make(20, 30, -90), 0.9);
	scene_AddLight(scene, &light);
	//light = light_make(vec3_make(20, 70, -80), 0.7);
	//scene_AddLight(scene, &light);
	scene->ambientCoefficient = 0.6;
	camera_init(&scene->camera, vec3_make(0, 5, -40), vec3_make(0, -0.4, 1), 800.0,
		scene->camera.width, scene->camera.height);

	Material screenMaterial = material_make(COLORS1_BLUE, 0, 0, 200);
	Vector3 vs[8];
	Surface floort[2] = {
		surface_initTriangle(vec3_make(-300,-10,-300), vec3_make(300,-10,-300), vec3_make(-300,-10,300),screenMaterial),
		surface_initTriangle(vec3_make(300,-10,-300), vec3_make(300,-10,300), vec3_make(-300,-10,300),screenMaterial)
	};
	scene_addSurfaceRange(scene, floort, 2);
}

