#include "bitmap.h"

CharacterAtlas* makeBitmaps(){
    /////// FREETYPE2 INIT
    FT_Library ft;
    if (FT_Init_FreeType(&ft)){
        printf("ERROR::FREETYPE: Could not init FreeType Library\n");
        exit(EXIT_FAILURE);
    }
    FT_Face face;
    if (FT_New_Face(ft, "res/CC.ttf", 0, &face)){
        printf("ERROR::FREETYPE: Failed to load font\n");
        exit(EXIT_FAILURE);
    }
    FT_Set_Pixel_Sizes(face, 0, GLYPH_SIZE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    glEnable(GL_BLEND);

    CharacterAtlas *atlas = malloc(sizeof(CharacterAtlas));

    for (unsigned char c = 0; c < 128; c++){
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
            printf("ERROR::FREETYTPE: Failed to load Glyph\n");
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // now store character for later use
      Character character = {
          texture,
          {face->glyph->bitmap.width, face->glyph->bitmap.rows},
          {face->glyph->bitmap_left, face->glyph->bitmap_top},
          face->glyph->advance.x
      };
      atlas->map[c] = character;
      atlas->hashmap[(int)c] = c; // trivial at this point
    }
    /// Clear glyphs
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return atlas;
}


