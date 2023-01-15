#ifndef __MAIN_H
#define __MAIN_H

//#include <iostream>
#include "gfx/gfx.h"

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include "gfx/vao.h"
#include "gfx/vbo.h"
#include "gfx/player.h"
#include "gfx/renderer.h"
#include "gfx/shader.h"
#include "gfx/bitmap.h"
#include "gfx/window.h"


//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netdb.h>
//#include <netinet/in.h>
//#include <errno.h>

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048
#define SERVER_PORT 10000


//#include <ft2build.h>
//#include FT_FREETYPE_H
//
//#include <fcntl.h>
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>


int main(int argc, char const *argv[]);

int window_width = 1920;
int window_height = 1080;

struct Window *window;

int flip_y = 1;

unsigned int screen_rbo;
unsigned int video_rbo;
unsigned int quadVAO, quadVBO;
unsigned int cubeVAO, cubeVBO;

struct Player *player;
struct Renderer *renderer;

float deltaTime, lastFrame;
bool animation=true;

//static void *get_proc_address(void *ctx, const char *name);
void processGLFWInput(GLFWwindow *window, mpv_handle *);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

float imgVertices[] = {
    // positions            // texCoords
    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f      , 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f,       1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
    1.0f, -1.0f, 0.0f,       1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,       1.0f, 1.0f};


//static void on_mpv_render_update(void *ctx);
//static void on_mpv_events(void *ctx);

int wakeup = 0;
//extern int wakeup;

void init();
void update();
void render();



bool SetSocketBlockingEnabled(int fd, bool blocking);


#endif
