#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

#include "stardude.h"

GLuint loadTex(char *fname, int alpha) {
	GLuint tid;

	printf("[load] %s: ", fname);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	glGenTextures(1, &tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	SDL_Surface *data = IMG_Load(fname);
	if(!data) {
		printf("fail\n");
		return 0;
	}
	else {
		printf("%ix%i\n", data->w, data->h);
	}
	if(alpha) {
		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, data->w, data->h, GL_RGBA,
			GL_UNSIGNED_BYTE, data->pixels );
	}
	else {
		gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, data->w, data->h, GL_RGB,
			GL_UNSIGNED_BYTE, data->pixels );
	}
	SDL_FreeSurface(data);

	return tid;
}

GLuint loadModel(char *fname) {
	FILE *fp;
	point3d mesh[1024];
	char line[64];
	int p=0;
	int v1, v2, v3, v4;
	int vt1, vt2, vt3, vt4;
	int vn1, vn2, vn3, vn4;
	GLuint lid;

	lid = glGenLists(1);
	if(!lid) return 0;

	printf("[load] %s: ", fname);

	fp = fopen(fname, "r");
	if(!fp) {
		printf("fail\n");
		return 0;
	}
	
	while(!feof(fp)) {
		fgets(line, 64, fp);
		if(sscanf(line, "v %f %f %f", &mesh[p].x, &mesh[p].y, &mesh[p].z)) p++;
	}
	fclose(fp);
	printf("%i vertices, ", p);
	
	p = 0;
	glNewList(lid, GL_COMPILE);
		glColor3f(0.7f, 0.7f, 0.7f);
		// glBindTexture(GL_TEXTURE_2D, tex_grass);
		glBegin(GL_QUADS);
			fp = fopen(fname, "r");
			while(!feof(fp)) {
				fgets(line, 64, fp);
				if(
					(sscanf(line, "f %i %i %i %i", &v1, &v2, &v3, &v4) == 4) ||
					(sscanf(line, "f %i//%i %i//%i %i//%i %i//%i",
						&v1, &vn1,
						&v2, &vn2,
						&v3, &vn3,
						&v4, &vn4) == 8) ||
					(sscanf(line, "f %i/%i/%i %i/%i/%i %i/%i/%i %i/%i/%i",
						&v1, &vt1, &vn1,
						&v2, &vt2, &vn2,
						&v3, &vt3, &vn3,
						&v4, &vt4, &vn4) == 12)
				) {
					v1--; v2--; v3--; v4--; // .obj counts from 1...
					// z and y are switched as blender uses +z=up,
					// but the game has -z=forwards
					glVertex3f(mesh[v1].x, mesh[v1].z, -mesh[v1].y);
					glVertex3f(mesh[v2].x, mesh[v2].z, -mesh[v2].y);
					glVertex3f(mesh[v3].x, mesh[v3].z, -mesh[v3].y);
					glVertex3f(mesh[v4].x, mesh[v4].z, -mesh[v4].y);
					p++;
				}
			}
			fclose(fp);
			printf("%i quads\n", p);
		glEnd();
	glEndList();

	return lid;
}

entity_t *genEntity(int modelid) {
	entity_t *o = malloc(sizeof(entity_t));
	o->model = modelid;
	o->x = 0; o->y = 0; o->z = 0;
	o->speed = 0;
	o->pitch = 0; o->yaw = 0; o->roll = 0;
	return o;
}

entity_t *cloneEntity(entity_t *src) {
	entity_t *o = malloc(sizeof(entity_t));
	o->model = src->model;
	o->x = src->x; o->y = src->y; o->z = src->z;
	o->speed = src->speed;
	o->pitch = src->pitch; o->yaw = src->yaw; o->roll = src->roll;
	return o;
}

