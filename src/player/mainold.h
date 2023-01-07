#ifndef __MAIN_H
#define __MAIN_H

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include <cstring>
#include <unistd.h>
#include <shader.h>
#include <map>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048
#define SERVER_PORT 10000


#include <ft2build.h>
#include FT_FREETYPE_H

#include <fcntl.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "danmaku.h"

class DanmakuHandler {
    public:

    private:

}

typedef struct Render {
    // Window info
    float h;
    float w;

    // Shaders
    Shader *screenShader;
    Shader *glyphShader;
    Shader *imgShader;

    // Danmakus
    std::map<char, Character> Characters;


} Render;

int window_width = 1366;
int window_height = 768;
int fbo_width = 1366;
int fbo_height = 768;

GLFWwindow *window = NULL;
mpv_handle *mpv;
mpv_render_context *mpv_ctx;

unsigned int video_framebuffer;
unsigned int video_textureColorbuffer;

unsigned int screen_framebuffer;
unsigned int screen_textureColorbuffer;

unsigned int screen_rbo;
unsigned int video_rbo;
unsigned int quadVAO, quadVBO;
unsigned int cubeVAO, cubeVBO;

float deltaTime, lastFrame;
unsigned int fcount = 0;
bool animation=true;

static void *get_proc_address(void *ctx, const char *name);
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

float quadVertices[] = {
    // positions            // texCoords
    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f      , 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f,       1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
    1.0f, -1.0f, 0.0f,       1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,       1.0f, 1.0f};


static void on_mpv_render_update(void *ctx);
static void on_mpv_events(void *ctx);
unsigned int wakeup = 0;



static inline void check_error(int);

bool SetSocketBlockingEnabled(int fd, bool blocking);

#endif
