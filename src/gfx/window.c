#include "window.h"

#include <stdio.h>
#include <stdlib.h>

static void _size_callback(GLFWwindow *handle, int width, int height) {
    glViewport(0, 0, width, height);
    window->size = (ivec2s) {{height, width}};

    window->size.x = width;
    window->size.y = height;
    glBindTexture(GL_TEXTURE_2D, renderer->screen_textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}


static void _error_callback(int code, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", code, description);
}

struct Window *window_create(FWindow init, FWindow destroy, FWindow tick,  FWindow update, FWindow render) {
    struct Window *window = malloc(sizeof(struct Window));

    window->init = init;
    window->destroy = destroy;
    window->tick = tick;
    window->update = update;
    window->render = render;

    window->last_frame = time(NULL);
    window->last_second = time(NULL);

    glfwSetErrorCallback(_error_callback);

    if (!glfwInit()){
        fprintf(stderr, "%s",  "error initializing GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window->size = (ivec2s) {{1920, 1080}};
    window->handle = glfwCreateWindow(window->size.x, window->size.y, WINDOW_NAME, NULL, NULL);
    if (window->handle == NULL) {
        fprintf(stderr, "%s",  "error creating window\n");
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window->handle);

    // configure callbacks
    glfwSetFramebufferSizeCallback(window->handle, _size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "%s",  "error initializing GLAD\n");
        glfwTerminate();
        exit(1);
    }

    glEnable(GL_DEPTH);
    glEnable(GL_MULTISAMPLE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwSwapInterval(1);

    return window;
}

static void _init() {
    window->init();
}

static void _destroy() {
    window->destroy();
    glfwTerminate();
}

static void _tick() {
    window->ticks++;
    window->tick();
}

static void _update() {
    window->update();
}

static void _render() {
    window->frames++;
    window->render();
}

void window_loop() {
    //_init();

    while (!glfwWindowShouldClose(window->handle)) {
        const u64 now = time(NULL);

        window->frame_delta = now - window->last_frame;
        window->last_frame = now;

        if (now - window->last_second > NS_PER_SECOND) {
            window->fps = window->frames;
            window->tps = window->ticks;
            window->frames = 0;
            window->ticks = 0;
            window->last_second = now;

            printf("FPS: %ld | TPS: %ld\n", window->fps, window->tps);
        }
        /* TEST FOR REPLACEMENET OF MAIN LOOP
        */


        /* END TEST FOR REPLACEMENET OF MAIN LOOP
        */


        //// tick processing
        //const u64 NS_PER_TICK = (NS_PER_SECOND / 60);
        //u64 tick_time = window->frame_delta + window->tick_remainder;
        //while (tick_time > NS_PER_TICK) {
        //    _tick();
        //    tick_time -= NS_PER_TICK;
        //}
        //window->tick_remainder = MAX(tick_time, 0);

        _update();
        _render();
        glfwSwapBuffers(window->handle);
        glfwPollEvents();
        usleep(16 * 1000); // we LIMIT the main render loop to 100FPS! If VSYSNC is enabled the limit is the VSYNC limit (~60fps)
    }

    _destroy();
    exit(0);
}

void mouse_set_grabbed(bool grabbed) {
    glfwSetInputMode(window->handle, GLFW_CURSOR, grabbed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool mouse_get_grabbed() {
    return glfwGetInputMode(window->handle, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}
