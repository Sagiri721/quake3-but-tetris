/**
 * @file        bitmap_text.c
 * @brief       Bitmap font text rendering
 */

#include "bitmap_text.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

size_t find_max(const char* chars, size_t num_chars) {
    size_t max_width = 0;
    for (size_t i = 0; i < num_chars; i++) {
        if (chars[i] > max_width) {
            max_width = chars[i];
        }
    }
    return max_width;
}

/**
 * This function builds a sparse buffer that maps character ASCII values
 * to their index in the font's character array.
 * so it will look something like this:
 * table['#'] = 2
 * table['A'] = 31
 * table['B'] = 32
 */
char build_character_jump_table(size_t* table, const char* chars, size_t num_chars) {

    assert(table != NULL);
    assert(chars != NULL);

    for(size_t i = 0; i < num_chars; i++)
        table[(unsigned char) chars[i]] = i;

    return 1;
}

char bitmap_font_init(bitmap_font* font, bitmap_desc desc) {

    assert(font != NULL);

    // Calculate characters per row
    desc.chars_per_row = desc.img_width_pixels / (desc.char_width_pixels + desc.char_padding_x_pixels);    

    font->desc = desc;

    size_t max_value = find_max(desc.chars, desc.num_chars);
    font->char_jump_table = (size_t*) malloc((max_value + 1) * sizeof(size_t));
    if (font->char_jump_table == NULL) {
        fprintf(stderr, "Failed to allocate memory for character jump table\n");
        return 0;
    }

    if (!build_character_jump_table(font->char_jump_table, desc.chars, desc.num_chars)){
        fprintf(stderr, "Failed to build character jump table\n");
        free(font->char_jump_table);
        return 0;
    }

    return 1;
}

void bitmap_font_destroy(bitmap_font* font) {
    
    assert(font != NULL);
    assert(font->char_jump_table != NULL);

    free(font->char_jump_table);
}

void bitmap_draw_char(bitmap_font* font, char c, sgp_rect rect) {
    
    assert(font != NULL);
    assert(font->char_jump_table != NULL);

    size_t char_index = font->char_jump_table[(size_t) c];
    int x_tiles = char_index % font->desc.chars_per_row;
    int y_tiles = char_index / font->desc.chars_per_row;

    int xx = x_tiles * (font->desc.char_width_pixels + font->desc.char_padding_x_pixels);
    int yy = y_tiles * (font->desc.char_height_pixels + font->desc.char_padding_y_pixels);

    sgp_draw_textured_rect(0, rect, (sgp_rect) {
        .x = (float) xx,
        .y = (float) yy,
        .w = (float) font->desc.char_width_pixels,
        .h = (float) font->desc.char_height_pixels
    });
}

void bitmap_draw_string(bitmap_font* font, const char* str, size_t len, sgp_rect rect) {
        
    assert(font != NULL);
    assert(font->char_jump_table != NULL);
    assert(str != NULL);

    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        bitmap_draw_char(font, c, (sgp_rect) {
            .x = rect.x + i * rect.w,
            .y = rect.y,
            .w = rect.w,
            .h = rect.h
        });
    }
}