#ifndef TEXTURE_H
#define TEXTURE_H

#include "gfx.h"
#include "../util/util.h"

struct Texture {
    GLuint handle;
    ivec2s size;
    vec2s uv_unit;
};

void texture_load_pixels(char *path, u8 **pixels_out, size_t *width_out, size_t *height_out);
struct Texture texture_create_from_pixels(u8 *pixels, size_t width, size_t height);
struct Texture texture_create_from_path(char *path);
void texture_destroy(struct Texture self);
void texture_bind(struct Texture self);
#endif
