#ifndef _RENDERER_H_
#define _RENDERER_H_

#include <mpv/render_gl.h>
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "bitmap.h"

#define SHADERS_LAST SHADER_BASIC_COLOR
enum ShaderType {
    SHADER_NONE = 0,
    SHADER_MPV,
    SHADER_GLYPH,
    SHADER_BASIC_COLOR,
    SHADER_BASIC_TEXTURE
};

extern int flip_y;


struct Renderer{
    struct Shader shaders[SHADERS_LAST + 1];
    enum ShaderType current_shader;
    struct Shader shader;


    // TODO: all of these should be more generically defined 
    struct VBO screenVBO;
    struct VAO screenVAO;

    unsigned int video_framebuffer;
    unsigned int video_textureColorbuffer;

    unsigned int screen_framebuffer;
    unsigned int screen_textureColorbuffer;

    unsigned int screen_rbo;
    unsigned int video_rbo;

    mpv_opengl_fbo mpv_fbo;
    mpv_render_param *params_fbo;

    struct VBO glyphVBO;
    struct VAO glyphVAO;

    CharacterAtlas *atlas;
};

struct Renderer* renderer_create();
void renderer_init(struct Renderer *self);
void renderer_destroy(struct Renderer *self);

float render_text(struct Renderer *self, const char* text, uint length, float x, float y, float scale, float color[3]);

#endif
