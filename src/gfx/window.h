#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "gfx.h"
#include "../util/util.h"
#include "./renderer.h"
#include "./player.h"

#define WINDOW_NAME "Wotos-mpv"

#include <unistd.h> // for usleep

#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

extern struct Player *player;
extern struct Renderer *renderer;
extern struct Window *window;

extern unsigned int screen_rbo;
extern int wakeup;

struct Button {
    bool down, last, last_tick, pressed, pressed_tick;
};


typedef void (*FWindow)();

struct Window {
    GLFWwindow *handle;
    ivec2s size;
    FWindow init, destroy, tick, update, render;

    // timing variables
    u64 last_second;
    u64 frames, fps, last_frame, frame_delta;
    u64 ticks, tps, tick_remainder;
};

void window_loop();
struct Window *window_create(FWindow init, FWindow destroy, FWindow tick,  FWindow update, FWindow render);
#endif
