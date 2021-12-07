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

#include "stardude.h"

static int doRender();
static int doInput();

static GLfloat zoom = -22.0f;

static GLuint tex_title;
static GLuint mdl_title;

static void doLoadMenu() {
	if(!tex_title) tex_title = loadTex("../textures/title.png", 0);

	mdl_title = glGenLists(1);
	glNewList(mdl_title, GL_COMPILE);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, tex_title);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  0.0f);	// Bottom Left Of The Texture and Quad
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  0.0f);	// Bottom Right Of The Texture and Quad
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  0.0f);	// Top Right Of The Texture and Quad
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  0.0f);  // top left
		glEnd();
	glEndList();
}

static void doUnloadMenu() {
	glDeleteLists(mdl_title, 1);
	//glDeleteTextures(1, &tex_title);
}

void doMenu() {
	doLoadMenu();
	while(gamemode == GM_MENU) {
		doRender();
		doFPS("Main Menu", 20);
		doInput();
	}
	doUnloadMenu();
}

/*
 * get OGL to draw the grid and the mesh
 */
int doRender() {
	if(zoom < -2) zoom += 0.5f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0.0f, zoom);
	glRotatef(360/20*(zoom+2),   1.0f, 0.0f, 0.0f);
    glRotatef(360/20*(zoom+2),   0.0f, 1.0f, 0.0f);

    glCallList(mdl_title);
	
    SDL_GL_SwapBuffers();
    
    return 1;
}

/*
 * handle SDL input
 */
int doInput() {
    SDL_Event event;

    SDL_PollEvent(&event);
    
    switch(event.type) {
	    case SDL_QUIT:
			gamemode = GM_EXIT;
			break;
	    case SDL_MOUSEBUTTONUP:
			if(event.button.button == SDL_BUTTON_LEFT) {
				printf("Menu click at %i,%i\n", event.button.x, event.button.y);
			}
			break;
	    case SDL_KEYDOWN:
			if(zoom < -2) {
		        switch (event.key.keysym.sym) {
					case SDLK_q: case SDLK_ESCAPE: gamemode = GM_EXIT; break;
					default: zoom = -2; break;
				}
			}
			else {
		        switch (event.key.keysym.sym) {
					case SDLK_q: case SDLK_ESCAPE: gamemode = GM_EXIT; break;
					case SDLK_UP:     break;
					case SDLK_DOWN:   break;
					case SDLK_LEFT:     break;
					case SDLK_RIGHT:    break;
					case SDLK_RETURN: gamemode = GM_GAME; break;
					default: break;
				}
			}
			break;
	}
	
	return 1;
}
