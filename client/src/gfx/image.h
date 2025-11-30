/**
 * @file        image.h
 * @brief       Image loading and handling
 */

#ifndef IMAGE_H
#define IMAGE_H

#include "../sokol_gp/thirdparty/sokol_gfx.h"
#include "../sokol_gp/sokol_gp.h"

typedef struct {
    int width, height;
    unsigned char* data;
    int num_channels;
} image_data;

image_data load_image_from_file(const char* filepath);

/**
 * Create a sokol-gfx image from raw image data
 * Does not own the image data !
 */
sg_image create_sg_image_from_data(image_data img_data);

/**
 * Create a sokol-gfx image directly from a file
 */
sg_image create_sg_image_from_file(const char* filepath);

void destroy_image_data(image_data* img_data);

#endif