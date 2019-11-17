// raytracer game window
// CSFML Graphics lib used to create 3D world experience
// Program: Elric Christopher
#include <sys/types.h>
#include <sys/stat.h>
void csfml_key_handler(Scene *scene, sfTime time, rt_keyboard *keyb, const float _force)
{
	//-
	if (keyb->walking.KeyW == true)
	{
		Vector3 arrow = vec3_spherical_to_cartesian((Vector3) {
				scene->camera.angle.x, 3.1415 / 2, _force
			});
			VEC3_SUB(scene->camera.position, scene->camera.position, arrow);
			camera_setup(&scene->camera);
	} 
	if (keyb->walking.KeyA == true)
	{
		Vector3 arrow = vec3_spherical_to_cartesian((Vector3) {
			scene->camera.angle.x + (3.1415 / 2), 3.1415 / 2, _force
		});
		VEC3_ADD(scene->camera.position, scene->camera.position, arrow);
		camera_setup(&scene->camera);
	}
	if (keyb->walking.KeyS == true)
	{
		Vector3 arrow = vec3_spherical_to_cartesian((Vector3) {
			scene->camera.angle.x, 3.1415 / 2, _force
		});
		VEC3_ADD(scene->camera.position, scene->camera.position, arrow);
		camera_setup(&scene->camera);
	}
	if (keyb->walking.KeyD == true)
	{
		Vector3 arrow = vec3_spherical_to_cartesian((Vector3) {
			scene->camera.angle.x - (3.1415 / 2), 3.1415 / 2, _force
		});
		VEC3_ADD(scene->camera.position, scene->camera.position, arrow);
		camera_setup(&scene->camera);
	}
	//-
	//-
	if (keyb->others.KeyE == true)
	{
		scene->camera.position.y += _force;
		camera_setup(&scene->camera);
	}
	if (keyb->others.KeyQ == true)
	{
		scene->camera.position.y -= _force;
		camera_setup(&scene->camera);
	}
	//-
	//-
	if (keyb->others.KeyR == true)
	{
		if(scene->ambientCoefficient < 0.9)scene->ambientCoefficient += 0.03;
	}
	else if (keyb->others.KeyF == true)
	{
		if(scene->ambientCoefficient > 0.1)scene->ambientCoefficient -= 0.03;
	}
	//-
}
void csfml_render(Raytracer *raytracer) {
	sfVideoMode mode;
	mode.width = (unsigned int)raytracer->resolutionX;
	mode.height = (unsigned int)raytracer->resolutionY;
	mode.bitsPerPixel = 32;
	sfIntRect bounds;
	bounds.top = 0;
	bounds.left = 0;
	bounds.width = (unsigned int)raytracer->resolutionX;
	bounds.height = (unsigned int)raytracer->resolutionY;
	sfRenderWindow *window;
	sfEvent event;
	sfImage *screen = sfImage_createFromColor(bounds.width, bounds.height, sfWhite);
	sfSprite *sprite = sfSprite_create();
	sfTexture *texture = sfTexture_createFromImage(screen, &bounds);
	sfClock *clock = sfClock_create();
	sfTime time;
	rt_keyboard keyb;
	memset(&keyb, false, sizeof(rt_keyboard));
	//-
	raytracer->hw_random = malloc(sizeof(Ray_XYRandom) * (raytracer->resolutionX * raytracer->resolutionY));
	if (!raytracer->hw_random) { printf("Error, Insufficient memory"); return; }
	FILE *file;
	if ((file = fopen("random.mem", "rb")) != NULL)
	{
		struct stat buf;
		fstat(fileno(file), &buf);
		if (buf.st_size != sizeof(Ray_XYRandom) * (raytracer->resolutionX * raytracer->resolutionY))
		{
			//File Corrupted || Resolution changed
			fclose(file);
			if (remove("random.mem"))return;
			goto Write_Rnd_File;
		}
		else
		{
			//File exists and size matches
			printf("\r\nLoad draw resources...");
			rewind(file);
			fread(raytracer->hw_random, sizeof(Ray_XYRandom), (raytracer->resolutionX * raytracer->resolutionY), file);
			fclose(file);
			printf("Done.\r\n");
		}
		
	}
	else //fájl nem létezik - létrehozás
	{
Write_Rnd_File:
		printf("\r Hianyzo render vezerlo fajl generalasa(with main thread)...");
		shuffle_hwrnd(raytracer, (raytracer->resolutionX * raytracer->resolutionY));
		file = fopen("random.mem", "wb");
		if (!file)return;
		fwrite(raytracer->hw_random, sizeof(Ray_XYRandom), (raytracer->resolutionX * raytracer->resolutionY), file);
		fflush(file);
		fclose(file);
		printf("Done.\r\n");
	}
	
	sfSprite_setTexture(sprite, texture, sfFalse);
	printf("\r\n Iranyitas:\r\nWalking: W-A-S-D\r\nFlying: KeyE-KeyQ\r\nLook_Around: Mouse\r\n");
	printf("Note: A programbol valo kilepes: ESC.\r\n\n");
	system("Pause");
	window = sfRenderWindow_create(mode, "Elric Christopher - raytracer 19.07.23 alpha MultiTH", sfClose, NULL);
	if (!window) {
		return;
	}
	screen = sfImage_createFromColor(bounds.width, bounds.height, sfWhite);
	//-
	raytracer_render_start(raytracer, (void*)screen);
	//-
	printf("Max FPS: 50.0\r\n");
	sfMouse_setPositionRenderWindow((sfVector2i) { raytracer->resolutionX / 2, raytracer->resolutionY / 2 }, window);
	sfRenderWindow_setMouseCursorVisible(window, sfFalse);
	while (sfRenderWindow_isOpen(window)) {
		sfClock_restart(clock);
		while (sfRenderWindow_pollEvent(window, &event)) {
			if (event.type == sfEvtClosed)
				sfRenderWindow_close(window);
			//Keyborad
			if (event.type == sfEvtKeyPressed || event.type == sfEvtKeyReleased)
			{
				switch (event.key.code)
				{
				case sfKeyEscape: sfRenderWindow_close(window); break;
				case sfKeyA: keyb.walking.KeyA = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyD: keyb.walking.KeyD = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyW: keyb.walking.KeyW = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyS: keyb.walking.KeyS = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyQ: keyb.others.KeyQ = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyE: keyb.others.KeyE = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyUp: keyb.Arrows.Up = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyDown: keyb.Arrows.Down = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyR: keyb.others.KeyR = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyF: keyb.others.KeyF = event.type == sfEvtKeyPressed ? true : false; break;
				/*case sfKeySpace: keyb.special.KeySpace = event.type == sfEvtKeyPressed ? true : false; break;
				case sfKeyLShift: keyb.special.KeyLShift = event.type == sfEvtKeyPressed ? true : false; break;*/
				default: break;
				}
			}
			//-
			//-Mouse
			if (event.type == sfEvtMouseMoved)
			{
				sfVector2i pos = sfMouse_getPositionRenderWindow(window);
				if (pos.x != (raytracer->resolutionX / 2) || pos.y != (raytracer->resolutionY / 2))
				{
					//position direction - focallengtht
					raytracer->scene.camera.angle.x -= ((double)(raytracer->resolutionX / 2) - pos.x) * .003;
					if((raytracer->scene.camera.angle.y + ((double)(raytracer->resolutionY / 2) - pos.y) * .003) > -3.1415 &&
						(raytracer->scene.camera.angle.y + ((double)(raytracer->resolutionY / 2) - pos.y) * .003) < 0)
					raytracer->scene.camera.angle.y += ((double)(raytracer->resolutionY / 2) - pos.y) * .003;
					set_camera_angle(&raytracer->scene.camera.direction, &raytracer->scene.camera.angle);
					sfMouse_setPositionRenderWindow((sfVector2i) { raytracer->resolutionX / 2, raytracer->resolutionY / 2 }, window);
					camera_setup(&raytracer->scene.camera);
				}
			}
			//-
		}
		//-

		//--move--
		csfml_key_handler(&raytracer->scene,sfClock_getElapsedTime(clock), &keyb, 1.8f); //force -> 1.8
		//-
		//wait some
		do
		{
			sfSleep((sfTime) { 5000 });
			time = sfClock_getElapsedTime(clock);
		} while (1 / sfTime_asSeconds(time) > 50.0f); //max 50.0
		//-
		sfTexture_updateFromImage(texture, screen, 0, 0);
		sfTexture_bind(texture);
		sfRenderWindow_drawSprite(window, sprite, NULL);
		sfRenderWindow_display(window);
		time = sfClock_getElapsedTime(clock);
		//printf("FPS: %f\r", 1/sfTime_asSeconds(time));
	}
	//-
	for (size_t i = 0; i < raytracer->_Threading._nprocs; i++)
	{
		*raytracer->_Threading._ray_threading[i]->_continue = false;
		sfThread_wait(raytracer->_Threading._threads[i]);
		free(raytracer->_Threading._ray_threading[i]->_continue);
		free(raytracer->_Threading._ray_threading[i]);
		sfThread_destroy(raytracer->_Threading._threads[i]);
	}
	//-
	if (raytracer->hw_random)free(raytracer->hw_random);
	sfImage_saveToFile(screen, "raytracer_ingame.png");
	sfImage_destroy(screen);
	sfTexture_destroy(texture);
	sfSprite_destroy(sprite);
	sfRenderWindow_destroy(window);
}
