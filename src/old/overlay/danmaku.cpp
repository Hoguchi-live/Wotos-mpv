#include "danmaku.h"

/// This is so fucking bad, just store everything and redraw with a translation in the shader holy shit
float RenderText(Shader s, std::string text, float x, float y, float scale, glm::vec3 color, uint &VAO, uint &VBO, std::map<char, Character>& Characters, float window_width)
{

    // activate corresponding render state
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    s.use();

    glUniform3f(glGetUniformLocation(s.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    Character ch;

    float xpos;
    float ypos;
    float w;
    float h;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        ch = Characters[*c];

        xpos = x + ch.Bearing.x * scale;
        ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        w = ch.Size.x * scale;
        h = ch.Size.y * scale;
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
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
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

//// Create bitmaps for text rendering
void makeBitmaps(std::map<char, Character>& Characters){
    ///////// FREETYPE2 INIT
    FT_Library ft;
    if (FT_Init_FreeType(&ft)){
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        exit(EXIT_FAILURE);
    }
    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/TTF/DejaVuSans.ttf", 0, &face)){
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        exit(EXIT_FAILURE);
    }
    FT_Set_Pixel_Sizes(face, 0, GLYPH_SIZE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    glEnable(GL_BLEND);


    for (unsigned char c = 0; c < 128; c++){
        // load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)){
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
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
          glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
          glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
          face->glyph->advance.x
      };
     Characters.insert(std::pair<char, Character>(c, character));
    }
    /// Clear glyphs
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

//TODO: Status <->

/*
    Sets nextRow to the index of the next free danmaku row and returns 1 if one is available, returns 0 otherwise.
   */
int getNextDanmakuRow(uint *ph_danmakuRows, int length, uint *nextRow) {
    for(int row = 0; row < length; row++) {
        if( (ph_danmakuRows)[row] ){
            *nextRow = row;
            return 1;
        }
    }
    return 0;
}

/*
   Flips the status of the danmaku row.
*/
int updateDanmakuRow(uint *ph_danmakuRows, uint row) {
    ph_danmakuRows[row] = !ph_danmakuRows[row];
    return 0;
}

/*
   Updates danmaku array according to the delta between last frame and current frame.
*/
int updateDanmakus(Render *r){

    // Amount of pixels to translate the danmakus by
    float deltaPix = DANMAKU_SPEED*delta;
    // Last glyph x-position (last pixel on the string)
    float lastPix = 0.0f;

    // Keep track of danmaku index
    uint danmakuIndex = 0;

    for(Danmaku &danmaku : danmakus) {
            if(danmaku.isNew) {
                // Try to assign the new danmaku's row to a free slot
                if(getNextDanmakuRow(ph_danmakuRows, nb_danmakuRows, &(danmaku.row))){
                    // Compute the height for that row
                    danmaku.ypos =  (r->h - DANMAKU_MARGIN_TOP) -
                                    (danmaku.row + 1) * (DANMAKU_PADDING_V + GLYPH_SIZE);
                    danmaku.xpos = r->w;
                    // Set the slot to unavailable
                    updateDanmakuRow(ph_danmakuRows, danmaku.row);
                    danmaku.tailCheck = 0;
                }
                danmaku.isNew = 0;
            }
            else {
                // Translate danmaku
                danmaku.xpos -= deltaPix;
            }

            // Draw the danmaku and get tail position
            lastPix = RenderText(r, danmaku.text, danmaku.xpos, danmaku.ypos, 1.0f, glm::vec3(0.0, 0.26f, 0.0f));
            if(lastPix + DANMAKU_PADDING_H < 0) {
                danmakus.erase(danmakus.begin() + danmakuIndex);
            }

            // Check if the tail of the danmaku is free  (only check once)
            if(!danmaku.tailCheck && (r->w > lastPix + (float)DANMAKU_PADDING_H)) {
                // Free the row slot
                updateDanmakuRow(ph_danmakuRows, danmaku.row);
                danmaku.tailCheck = 1;
            }
        }
        danmakuIndex++;
    }
    return 1;
}


OverlayHandler() {
    init();
}

int OverlayHandler::init() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

}
