// 3D raytracer main.c : Elric Christopher
// Ez a program egy 3D virtuális motor, melyben
// a létrehozott világ minden részletét matematikai-trigonometriai alapon,
// a CPU dolgoz fel, egyszerre több szálon.
#pragma warning(disable : 4996)
//-
#include <Windows.h>
#include <errno.h>
//-
#include <immintrin.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <float.h>
#include <time.h>
#include <SFML/Audio.h>
#include <SFML/Graphics.h>
#include <SFML/Graphics/RenderWindow.h>
#include "func_header.h"

int main(int argc, const char *argv[])
{
	long nprocs = -1;
	long nprocs_max = -1;

	Raytracer rytr;

	printf("3D Raytracer by: Elric Christopher @2019\r\n");

#ifdef _WIN32
#ifndef _SC_NPROCESSORS_ONLN
	SYSTEM_INFO info;
	GetSystemInfo(&info);
#define sysconf(a) info.dwNumberOfProcessors
#define _SC_NPROCESSORS_ONLN
#endif
#endif
#ifdef _SC_NPROCESSORS_ONLN
	nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	if (nprocs < 1)
	{
		fprintf(stderr, "Could not determine number of CPUs online:\n%s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}
	nprocs_max = sysconf(_SC_NPROCESSORS_CONF);
	if (nprocs_max < 1)
	{
		fprintf(stderr, "Could not determine number of CPUs configured:\n%s\n",
			strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("%ld of %ld processors online\n", nprocs, nprocs_max);
	int felb = 0; 
	/*do {*/
	printf("Válassz render felbontást:\r\n");
	printf("-(1) 640x480\r\n");
	printf("-(2) 800x600\r\n");
	printf("-(3) 1280x1024\r\n"); //egyelõre elég ez a 3.
	//scanf("%i", &felb);
	//} while (felb <= 0 || felb > 3); //nahát ezt még megoldani...mi a k...
	switch (getchar()) //nahát ezt még megoldani...mi a k...
	{
	case '1': 
		raytracer_init(&rytr, 640, 480, nprocs);
		printf("640");
		break;
	case '2': 
		raytracer_init(&rytr, 800, 600, nprocs);
		break;
	case '3': 
		raytracer_init(&rytr, 1280, 1024, nprocs);
		break;
	default:
		raytracer_init(&rytr, WINDOW_WIDTH, WINDOW_HEIGHT, nprocs);
		printf("def");
		break;
	}
	select_Land(argc, argv, &rytr.scene);
	csfml_render(&rytr);
	raytracer_dealloc(&rytr);
	exit(EXIT_SUCCESS);
#else
	fprintf(stderr, "Could not determine number of CPUs");
	exit(EXIT_FAILURE);
#endif
	system("Pause");
	exit(EXIT_SUCCESS);
}