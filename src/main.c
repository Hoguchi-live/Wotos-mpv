#include "main.h"
#include "gfx/vao.h"
#include "gfx/vbo.h"
#include "glad/glad.h"

static inline void check_error(int); 
static inline void _setup();

int main(int argc, char const *argv[]){
    if (argc < 2){ return -1;}
    _setup();

    player_loadfile(player, argv[1]);
    window_loop();
    return 0;
}

void _setup(){
    window = window_create(init, NULL, NULL, update, render);

    player = player_create();
    player_init(player);

    renderer = renderer_create();
    renderer_init(renderer); 

    /* Glyph shenanigans */
    //mat4s transformation = glms_ortho(0.0f, (float)(window_width), 0.0f, (float)window_height, -100, 100);
}

// TODO Remove these function from here 
void processGLFWInput(GLFWwindow *window, mpv_handle *ctx)
{
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        const char *c[] = {"show-text", "lol", NULL};
        check_error(mpv_command(ctx, c));
    }

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // we have to rescale the Texture and renderbuffer storage.
    window_height = height;
    window_width = width;
    glBindTexture(GL_TEXTURE_2D, renderer->screen_textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window_width, window_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, screen_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, window_width, window_height);
}

static inline void check_error(int status)
{
    if (status < 0) {
        printf("mpv API error: %s\n", mpv_error_string(status));

        //exit(1);
    }
}

void init() {
    
}

void update() {
    processGLFWInput(window->handle, player->handle);

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
