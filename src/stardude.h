#ifndef _STARDUDE_H_
#define _STARDUDE_H_
/*
 * stardude.h (c) Shish 2005
 * 
 * things common to the viewer and generators
 */
#include <GL/gl.h>

#ifdef __WIN32
  #include <windows.h>
  #define LOADLIB(path)      LoadLibrary(path)
  #define GETFUNC(lib, func) GetProcAddress(lib, func)
  #define FREELIB(lib)       FreeLibrary(lib)
#else
  #include <dlfcn.h>
  #define LOADLIB(path)      dlopen(path, RTLD_NOW)
  #define GETFUNC(lib, func) dlsym(lib, func)
  #define FREELIB(lib)       dlclose(lib)
#endif

#define WIDTH 640
#define HEIGHT 480

typedef struct {
	float x, y, z;
	float r, g, b;
    int draw;
} point3d;

typedef struct {
	GLuint model;
	GLfloat x, y, z;
	GLfloat speed;
	GLfloat pitch, yaw, roll;
} entity_t;


typedef enum {
	GM_INIT,
	GM_INTRO,
	GM_MENU,
	GM_GAME,
	GM_CREDIT,
	GM_EXIT,
	GM_COUNT
} gamemode_e;

extern gamemode_e gamemode;

GLuint loadTex(char *fname, int alpha);
GLuint loadModel(char *fname);
entity_t *genEntity(int modelid);
entity_t *cloneEntity(entity_t *src);

void doInit();
void doMenu();
void doGame();

int doFPS(char *status, int target);

#endif
