#include "renderer.h"
#include "shader.h"
#include "vao.h"
#include "vbo.h"

void _renderer_mpv(struct Renderer *self);
void _renderer_glyph(struct Renderer *self);

float quadVertices[] = {
    // positions            // texCoords
    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f      , 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f,       1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f,       0.0f, 1.0f,
    1.0f, -1.0f, 0.0f,       1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,       1.0f, 1.0f
};

int fbo_width = 1920;
int fbo_height = 1080;

struct Renderer* renderer_create() {
    return malloc(sizeof(struct Renderer));
}

void renderer_init(struct Renderer *self) {
    self->current_shader = SHADER_NONE;

    // Compile shaders
    self->shaders[SHADER_MPV] = shader_create(
        "res/shaders/screen_vs.glsl", "res/shaders/screen_fs.glsl",
        2, (struct VertexAttr[]){
            { .index = 0, .name = "pos" },
            { .index = 1, .name = "texcoord" }
        });

    self->shaders[SHADER_GLYPH] = shader_create(
        "res/shaders/glyph_vs.glsl", "res/shaders/glyph_fs.glsl",
        2, (struct VertexAttr[]){
            { .index = 0, .name = "texcoord" }
        });

    _renderer_mpv(self);
    _renderer_glyph(self);

    self->atlas = makeBitmaps();
}

void renderer_destroy(struct Renderer *self) {
    for (size_t i = 0; i <= SHADERS_LAST; i++) {
        shader_destroy(self->shaders[i]);
    }

    vao_destroy(self->screenVAO);
    vbo_destroy(self->screenVBO);

    free(self->params_fbo);
    free(self);
}

/*  
    Setting up the glyph renderer (for text)
*/
void _renderer_glyph(struct Renderer *self) {
    self->glyphVAO = vao_create();
    self->glyphVBO = vbo_create(GL_ARRAY_BUFFER, true);

    vao_bind(self->glyphVAO);
    vbo_bind(self->glyphVBO);

    vbo_buffer(self->glyphVBO, &quadVertices, 0, sizeof(float)*6*4);
    vao_attr(self->glyphVAO, self->glyphVBO, 0, 4, GL_FLOAT, 4*sizeof(float), 0);
}

/*  
    Setting up the mpv renderer
*/
void _renderer_mpv(struct Renderer *self) {

    /* Screen VAO and VBO */
    self->screenVAO = vao_create();
    self->screenVBO = vbo_create(GL_ARRAY_BUFFER, false);

    vao_bind(self->screenVAO);
    vbo_bind(self->screenVBO);

    vbo_buffer(self->screenVBO, &quadVertices, 0, sizeof(quadVertices));
    vao_attr(self->screenVAO, self->screenVBO, 0, 3, GL_FLOAT, 5*sizeof(float), 0);
    vao_attr(self->screenVAO, self->screenVBO, 1, 2, GL_FLOAT, 5*sizeof(float), (3 * sizeof(float)));

    /* Framebuffer for Video */
    glGenFramebuffers(1, &(self->video_framebuffer));
    glBindFramebuffer(GL_FRAMEBUFFER, self->video_framebuffer);

    /* create a color attachment texture */
    glGenTextures(1, &(self->video_textureColorbuffer));
    glBindTexture(GL_TEXTURE_2D, self->video_textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo_width, fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->video_textureColorbuffer, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("ERROR::FRAMEBUFFER:: VIDEO Framebuffer #%d is not complete!\n", self->video_framebuffer);
    }
    self->mpv_fbo = (mpv_opengl_fbo){(int)(self->video_framebuffer), fbo_width, fbo_height, 0};

    self->params_fbo = malloc(sizeof(mpv_render_param [3]));
    memcpy(self->params_fbo, (mpv_render_param [3]){
        {MPV_RENDER_PARAM_OPENGL_FBO, &(self->mpv_fbo)},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, NULL}}, sizeof(mpv_render_param [3]));

}

/*
   Render text on the screen using the given Shader
*/
float render_text(struct Renderer *self, const char* text, uint length, float x, float y, float scale, float color[3])
{
    shader_bind(self->shaders[SHADER_GLYPH]);
    glUniform3f(glGetUniformLocation((self->shaders[SHADER_GLYPH]).handle, "textColor"), color[0], color[1], color[2]);
    glActiveTexture(GL_TEXTURE0);
    vao_bind(self->glyphVAO);

    Character ch;

    float xpos;
    float ypos;
    float w;
    float h;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // iterate through all characters
    for (unsigned char c = 0; c < length; c++){
        ch = self->atlas->map[self->atlas->hashmap[(int)text[c]]];

        xpos = x + ch.Bearing[0] * scale;
        ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

        w = ch.Size[0] * scale;
        h = ch.Size[1] * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        vbo_bind(self->glyphVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return xpos;
}
