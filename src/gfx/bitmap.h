#ifndef _BITMAP_H_
#define _BITMAP_H_

#include "gfx.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "stdlib.h"
#include "stdio.h"

#include "../util/ivec2.h"

#define GLYPH_SIZE 36
#define DANMAKU_MARGIN_TOP 50
#define DANMAKU_MARGIN_BOT 50
#define DANMAKU_PADDING_H 200
#define DANMAKU_PADDING_V 20
#define DANMAKU_SPEED_V 200.0f

#define MAP_SIZE 128

// Glyphs
typedef struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    ivec2   Size;       // Size of glyph
    ivec2   Bearing;    // Offset from baseline to left/top of glyph
    long int Advance;    // Offset to advance to next glyph
} Character;

typedef struct CharacterAtlas {
    Character map[MAP_SIZE];
    int size;
    int hashmap[MAP_SIZE];
} CharacterAtlas;

CharacterAtlas *makeBitmaps();


#endif
