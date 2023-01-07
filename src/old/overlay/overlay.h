#ifndef _OVERLAY_H_
#define _OVERLAY_H_

#include <shader.h>
#include <map>
#include <vector>
#include "stdlib.h"
#include "stdio.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define GLYPH_SIZE 36
#define DANMAKU_MARGIN_TOP 50
#define DANMAKU_MARGIN_BOT 50
#define DANMAKU_PADDING_H 200
#define DANMAKU_PADDING_V 20
#define DANMAKU_SPEED_V 200.0f

// Glyphs
typedef struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    long int Advance;    // Offset to advance to next glyph
} Character;

// Danmaku
typedef struct Danmaku {
    std::string text;
    uint isActive;
    float xpos;
    float ypos;
    uint isNew;
    uint row;
    uint tailCheck;
} Danmaku;

class Overlay() {
    public:
        OverlayHandler();
        int init();

    private:
        void makeBitmaps(std::map<char, Character> &);
}

#endif
