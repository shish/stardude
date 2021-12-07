/*
 * main.c (c) Shish 2005
 * 
 * StarDude, blow stuff up!
 */

#include <SDL/SDL.h>

#include "stardude.h"

gamemode_e gamemode;


/*
 * start, main loop, end
 */
int main(int argc, char *argv[]) {
	gamemode = GM_INIT;
	
	while(gamemode != GM_EXIT) {
		switch(gamemode) {
			case GM_INIT:
				doInit();
				break;
			case GM_MENU:
				doMenu();
				break;
			case GM_GAME:
				doGame();
				break;
			default:
				printf("Unknown game mode!\n");
				gamemode = GM_EXIT;
				break;
		}
	}

	SDL_Quit();
	return 1;
}


/*
 * calculate average FPS over the last 8 frames
 */
int doFPS(char *status, int target) {
	static int start = 0;
	static int now = 0;
	static int delta = 0;
	static int last[8];
	static int frame = 0;
	
	char title[128];
	int ave = 1;
	
	now = SDL_GetTicks();
	delta = now-start;
	start = SDL_GetTicks();
	
	last[frame++%8] = delta;
	
	ave = (last[0]+last[1]+last[2]+last[3]+
	       last[4]+last[5]+last[6]+last[7])/8;

	if(ave) {
		sprintf(title, "%s 0.0 %ifps", status, 1000/ave);
		SDL_WM_SetCaption(title, NULL);
	}

	// anti-aliasing off and no fps cap = 200FPS on slow hardware~
	if(1000/target-ave > 0) SDL_Delay(1000/target-ave);
	
	return 1;
}

