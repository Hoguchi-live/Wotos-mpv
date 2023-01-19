#ifndef SHADER_H
#define SHADER_H

#include "gfx.h"
#include "texture.h"
#include "../util/util.h"

struct VertexAttr {
    GLuint index;
    const GLchar *name;
};

struct Shader {
    GLuint handle, vs_handle, fs_handle;
};

struct Shader shader_create(char *vs_path, char *fs_path, size_t n, struct VertexAttr attributes[]);
void shader_destroy(struct Shader self);

void shader_bind(struct Shader self);

#endif
