#include "window_setup.h"

static inline void _check_error(int); 
static inline void _processGLFWInput(GLFWwindow *window, mpv_handle *ctx);


void init() {
}

void destroy() {
}

void tick() {
}

void update() {
    _processGLFWInput(window->handle, player->handle);

    if (wakeup)
    {
        if ((mpv_render_context_update(player->ctx) & MPV_RENDER_UPDATE_FRAME))
        {
            mpv_render_context_render(player->ctx, renderer->params_fbo);                 
            glViewport(0, 0, window->size.x, window->size.y);
        }
    }
}

void render(){
    shader_bind(renderer->shaders[SHADER_MPV]);

    vao_bind(renderer->screenVAO);
    glBindTexture(GL_TEXTURE_2D, renderer->video_textureColorbuffer); // <-- SCREEN Colorbuffer IS THE TEXTURE
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // -----
    if (wakeup)
    {
        mpv_render_context_report_swap(player->ctx);
        wakeup = 0;
    }

    const char *text = "I Joe you Joe ;)";
    render_text(renderer, text, strlen(text), 0, 0, 0.001, (float [3]){0.0, 1.0, 0.0});
}

void _processGLFWInput(GLFWwindow *window, mpv_handle *ctx)
{
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        const char *c[] = {"show-text", "lol", NULL};
        _check_error(mpv_command(ctx, c));
    }

}

static inline void _check_error(int status)
{
    if (status < 0) {
        printf("mpv API error: %s\n", mpv_error_string(status));

        //exit(1);
    }
}

