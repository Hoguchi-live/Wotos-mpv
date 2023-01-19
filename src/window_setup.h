#ifndef _WINDOW_SETUP_H_
#define _WINDOW_SETUP_H_

#include "gfx/player.h"
#include "gfx/renderer.h"
#include "gfx/window.h" // Also imports player.h and renderer.h

/* 
   Global variables
*/
extern struct Window *window;
extern struct Player *player;
extern struct Renderer *renderer;

extern int wakeup;

/*
    Window functions 
*/
void init();
void destroy();
void tick();
void update();
void render();

#endif
