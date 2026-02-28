/**
 * @file        text.h
 * @brief       Bitmap font rendering
 *              with help from https://www.youtube.com/watch?v=j9AmNYUuA2Q
 */

#ifndef BITMAP_TEXT_H
#define BITMAP_TEXT_H

#include "../sokol_gp/thirdparty/sokol_gfx.h"
#include "../sokol_gp/sokol_gp.h"

/**
 * Description of a bitmap font
 * chars_per_row is calculated on init
 */
typedef struct {
    sg_image img;
    
    unsigned int img_width_pixels;
    unsigned int img_height_pixels;
    unsigned int chars_per_row;
    
    unsigned int char_width_pixels;
    unsigned int char_height_pixels;

    unsigned int char_padding_x_pixels;
    unsigned int char_padding_y_pixels;

    float aspect_ratio;

    const char* chars;
    size_t num_chars;

} bitmap_desc;

/**
 * A bitmap font
 */
typedef struct {
    bitmap_desc desc;
    size_t* char_jump_table;
} bitmap_font;

char bitmap_font_init(bitmap_font* font, bitmap_desc desc);
void bitmap_font_destroy(bitmap_font* font);

void bitmap_draw_char(bitmap_font* font, char c, sgp_rect rect);
void bitmap_draw_string(bitmap_font* font, const char* str, size_t len, sgp_rect rect);

#endif