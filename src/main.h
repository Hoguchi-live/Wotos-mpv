#ifndef _MAIN_H_
#define _MAIN_H_

#include "window_setup.h"

#include "connection/sock.h"
#include <fcntl.h>

#include <mpv/client.h>
#include <mpv/render_gl.h>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>


int main(int argc, char const *argv[]);

/*
   Global variables
*/
struct Socket *sock;
struct Window *window;
struct Player *player;
struct Renderer *renderer;

int flip_y = 1;
unsigned int screen_rbo;
unsigned int video_rbo;

int wakeup = 0;

//float imgVertices[] = {
//    // positions            // texCoords
//    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
//    -1.0f, -1.0f, 0.0f      , 0.0f, 0.0f,
//    1.0f, -1.0f, 0.0f,       1.0f, 0.0f,
//
//    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
//    1.0f, -1.0f, 0.0f,       1.0f, 0.0f,
//    1.0f, 1.0f, 0.0f,       1.0f, 1.0f};



#endif
