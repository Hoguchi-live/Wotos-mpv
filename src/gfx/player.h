#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"
#include <mpv/client.h>
#include <mpv/render_gl.h>

struct Player {
    mpv_handle *handle;
    mpv_render_context *ctx;
};

struct Player* player_create();
void player_init(struct Player *self);
void player_destroy(struct Player *self);

#endif
