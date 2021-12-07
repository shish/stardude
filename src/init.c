/*
 * menu.c (c) Shish 2005
 * 
 * Select options and such
 */

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

int initSDL();
int initOpenGL();
int initLists();
int initTex();

void doInit() {
	if(initSDL() && initOpenGL() && initTex()) {
		gamemode = GM_MENU;
	}
	else {
		SDL_Quit();
		gamemode = GM_EXIT;
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                            *
*                       Setup / Initialisation Stuff                         *
*                                                                            *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Boilerplate SDL+OGL init
 */
int initSDL() {
    const SDL_VideoInfo* video;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
        return 0;
    }
    atexit(SDL_Quit);

    video = SDL_GetVideoInfo();
    if(!video) {
        fprintf(stderr, "Couldn't get video information: %s\n", SDL_GetError());
        return 0;
    }

    /* Set the minimum requirements for the OpenGL window */
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    if(SDL_SetVideoMode(WIDTH, HEIGHT, video->vfmt->BitsPerPixel, SDL_OPENGL) == 0) {
        fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
        return 0;
    }
    SDL_WM_SetCaption("StarDude Loading...", NULL);

	return 1;
}

/*
 * Boilerplate OGL init
 */
int initOpenGL() {
    float aspect = (float)WIDTH / (float)HEIGHT;
    glViewport(0, 0, WIDTH, HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(1.0, 1.0, 1.0, 0);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LINE_SMOOTH); // antialiasing
    glDisable(GL_CULL_FACE); // remove faces that don't face the camera
	return 1;
}

int initTex() {
	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// SDL + OGL makes textures go backwards
	glMatrixMode(GL_TEXTURE);
	glRotatef(180.0f,0.0f,0.0f,1.0f);
	glScalef(-1.0f,1.0f,1.0f);
	glMatrixMode(GL_MODELVIEW);

	return 1;
}

