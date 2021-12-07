/*
 * game.c (c) Shish 2005
 * 
 * The main game loop
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

static int time = 0;
static GLfloat zoom = -10;
static char keys[256];


#define MAX_ENTITIES 1024
entity_t *entities[MAX_ENTITIES];

static int doPhysics();
static int doRender();
static int doInput();

static GLuint tex_grass, tex_crosshair;
static GLuint mdl_player, mdl_laser, mdl_ufo, mdl_crosshair, mdl_grass, mdl_blast;

static void addEntity(entity_t *e) {
	int i = 0;
    for(i=0; i<MAX_ENTITIES; i++) {
		if(entities[i] == NULL) {
			printf("[entities] Added to slot %i\n", i);
			entities[i] = e;
			break;
		}
	}
	if(i == MAX_ENTITIES) {
		printf("[entities] WARNING: All slots full\n");
	}
}

static void doLoadGame() {
	tex_grass     = loadTex("../textures/grass.jpg",     0);
	tex_crosshair = loadTex("../textures/crosshair.png", 1);

	mdl_player = loadModel("../objects/player.obj");
	mdl_laser  = loadModel("../objects/laser.obj");
	mdl_ufo    = loadModel("../objects/ufo.obj");
	mdl_blast  = loadModel("../objects/blast.obj");


    for(int i=0; i<MAX_ENTITIES; i++) {
		entities[i] = NULL;
	}
	entities[0] = genEntity(mdl_player);

    for(int i=0; i<256; i++) {keys[i] = 0;}

	mdl_grass = glGenLists(1);
	glNewList(mdl_grass, GL_COMPILE);
		glColor3f(0.7f, 0.7f, 0.7f);
		glBindTexture(GL_TEXTURE_2D, tex_grass);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f,   0.0f); glVertex3f(-20, -10, -10);
			glTexCoord2f(20.0f,  0.0f); glVertex3f( 20, -10, -10);
			glTexCoord2f(20.0f, 10.0f); glVertex3f( 20, -10,  10);
			glTexCoord2f(0.0f,  10.0f); glVertex3f(-20, -10,  10);
		glEnd();
	glEndList();

//		glEnable(GL_BLEND);		// Turn Blending On
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE);	
//		glDisable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0.6);
	glEnable(GL_ALPHA_TEST);
     
	mdl_crosshair = glGenLists(1);
	glNewList(mdl_crosshair, GL_COMPILE);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, tex_crosshair);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -3);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -3);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -3);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -3);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -6);
			glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -6);
			glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -6);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -6);
		glEnd();
	glEndList();
	
	entity_t *grassbase = genEntity(mdl_grass);
	for(int i=0; i<20; i++) {
		grassbase->z = -(i*20);
		addEntity(cloneEntity(grassbase));
	}
	free(grassbase);
}

static void doUnloadGame() {
	glDeleteLists(mdl_player, 1);
	glDeleteLists(mdl_laser, 1);
	//glDeleteLists(mdl_ufo, 1);
	glDeleteLists(mdl_blast, 1);
	glDeleteLists(mdl_grass, 1);
	glDeleteTextures(1, &tex_grass);
	glDeleteTextures(1, &tex_crosshair);
	
    for(int i=0; i<MAX_ENTITIES; i++) {
		if(entities[i]) free(entities[i]);
	}
}

void doGame() {
	doLoadGame();
	while(gamemode == GM_GAME) {
		doPhysics();
		doRender();
		doFPS("StarDude", 30);
		doInput();
	}
	doUnloadGame();
}

static GLfloat tendToZero(GLfloat f) {
	while(f < 180) f += 360;
	while(f > 180) f -= 360;
	if(f < -1.1) f++;
	if(f >  1.1) f--;
	return f;
}

int doPhysics() {
    time++;
    
    if(keys[SDLK_LEFT]) entities[0]->yaw++;
    else if(keys[SDLK_RIGHT]) entities[0]->yaw--;
    else entities[0]->yaw = tendToZero(entities[0]->yaw);
    
    if(keys[SDLK_RCTRL]) entities[0]->roll++;
    else if(keys[SDLK_KP0]) entities[0]->roll--;
    else entities[0]->roll = tendToZero(entities[0]->roll);

    if(keys[SDLK_UP]) entities[0]->pitch--;
    else if(keys[SDLK_DOWN]) entities[0]->pitch++;
    else entities[0]->pitch = tendToZero(entities[0]->pitch);


    GLfloat m = 3.14159f/180.0f;
    for(int i=0; i<MAX_ENTITIES; i++) {
		if(entities[i]) {
			GLfloat s = entities[i]->speed;
			entities[i]->x -= sin(entities[i]->yaw*m)*s;
			entities[i]->y += sin(entities[i]->pitch*m)*s;
			entities[i]->z -= s; // cos(entities[i]->yaw/m)*s;
			
			if(entities[i]->model == mdl_laser) {
				if(entities[i]->z < entities[0]->z-128) {
					free(entities[i]);
					entities[i] = NULL;
					printf("[entities] Laser hit +128 %i\n", i);
				}
				else if(entities[i]->y <= -10) {
					entities[i]->model = mdl_blast;
					entities[i]->speed = 0;
					printf("[entities] laser turned to blast (%i)\n", i);
				}
			}
			else if(entities[i]->x > 16 || entities[i]->x < -16) {
				free(entities[i]);
				entities[i] = NULL;
				printf("[entities] E%i OOB(x)\n", i);
			}
			else if(entities[i]->y > 16 || entities[i]->y < -16) {
				free(entities[i]);
				entities[i] = NULL;
				printf("[entities] E%i OOB(y)\n", i);
			}
			else if(entities[i]->z > entities[0]->z+16) {
				free(entities[i]);
				entities[i] = NULL;
				printf("[entities] E%i OOB(z)\n", i);
			}
		}
	}
    
	if(zoom < -30) zoom = -30;
	else if(zoom > -3) zoom = -3;

    return 1;
}

/*
 * get OGL to draw the grid and the mesh
 */
int doRender() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	if(zoom == -3) { // cockpit view
	    glCallList(mdl_crosshair);

		glTranslatef(-entities[0]->x, -entities[0]->y, -entities[0]->z);
	    glRotatef(entities[0]->pitch, -1.0f,  0.0f,  0.0f);
	    glRotatef(entities[0]->yaw,    0.0f, -1.0f,  0.0f);
	    glRotatef(entities[0]->roll,   0.0f,  0.0f, -1.0f);
	}
	else { // external view
	    glTranslatef(0.0f, 0.0f, -entities[0]->z+zoom);

		glPushMatrix();
		glTranslatef(entities[0]->x, entities[0]->y, entities[0]->z);
	    glRotatef(entities[0]->pitch, 1.0f, 0.0f, 0.0f);
	    glRotatef(entities[0]->yaw,   0.0f, 1.0f, 0.0f);
	    glRotatef(entities[0]->roll,  0.0f, 0.0f, 1.0f);
		glCallList(mdl_player);
		glCallList(mdl_crosshair);
		glPopMatrix();
	}

    for(int i=1; i<MAX_ENTITIES; i++) {
		if(entities[i] &&
				(entities[i]->z > entities[0]->z-64) &&
				(entities[i]->z < entities[0]->z+16)
				) {
			glPushMatrix();
			glTranslatef(entities[i]->x, entities[i]->y, entities[i]->z);
		    glRotatef(entities[i]->pitch, 1.0f, 0.0f, 0.0f);
		    glRotatef(entities[i]->yaw,   0.0f, 1.0f, 0.0f);
		    glRotatef(entities[i]->roll,  0.0f, 0.0f, 1.0f);
		    glCallList(entities[i]->model);
			glPopMatrix();
		}
	}

    SDL_GL_SwapBuffers();
    
    return 1;
}

static void fire() {
	static int left = 0;
	entity_t *laser = cloneEntity(entities[0]);
	laser->model = mdl_laser;
	laser->z++;
	left ? laser->x-- : laser->x++;
	left = !left;
	laser->speed += 1;
	addEntity(laser);
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
	    case SDL_MOUSEBUTTONDOWN:
			switch(event.button.button) {
				case SDL_BUTTON_LEFT: fire(); break;
				case SDL_BUTTON_WHEELUP:   entities[0]->speed += 0.1; break;
				case SDL_BUTTON_WHEELDOWN: entities[0]->speed -= 0.1; break;
			}
			break;
	    case SDL_MOUSEMOTION:
			if(event.motion.state & (SDL_BUTTON(2)|SDL_BUTTON(3))) {
				zoom -= (float)event.motion.yrel/4.0f;
			}
			else {
				entities[0]->yaw += -event.motion.xrel;
				entities[0]->pitch += -event.motion.yrel;
				entities[0]->roll += -event.motion.xrel;
			}
			break;
	    case SDL_KEYDOWN:
			keys[event.key.keysym.sym] = 1;
			break;
	    case SDL_KEYUP:
			keys[event.key.keysym.sym] = 0;
			break;
			/*
	        switch (event.key.keysym.sym) {
				case SDLK_q: case SDLK_ESCAPE: gamemode = GM_MENU; break;
				case SDLK_w: case SDLK_KP8: case SDLK_UP:    entities[0]->pitch++; break;
				case SDLK_s: case SDLK_KP2: case SDLK_DOWN:  entities[0]->pitch--; break;
				case SDLK_a: case SDLK_KP4: case SDLK_LEFT:  entities[0]->yaw++;   break;
				case SDLK_d: case SDLK_KP6: case SDLK_RIGHT: entities[0]->yaw--;   break;
				default: break;
			}
			break;
			*/
	}

	for(int i=0; i<256; i++) {	
		if(keys[i]) {
			switch(i) {
				case SDLK_q: case SDLK_ESCAPE: gamemode = GM_MENU; break;
				case SDLK_w: case SDLK_KP8: case SDLK_UP:    entities[0]->pitch++; break;
				case SDLK_s: case SDLK_KP2: case SDLK_DOWN:  entities[0]->pitch--; break;
//				case SDLK_a: case SDLK_KP4: case SDLK_LEFT:  entities[0]->yaw++;   break;
//				case SDLK_d: case SDLK_KP6: case SDLK_RIGHT: entities[0]->yaw--;   break;
			}
		}
	}

	return 1;
}
