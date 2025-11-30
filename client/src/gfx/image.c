/**
 * @file        image.c
 * @brief       Image loading and handling
 */

#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const int RGBA_CHANNELS = 4;

image_data load_image_from_file(const char* filepath) {

    int w,h, chn;
    unsigned char* data = stbi_load(filepath, &w, &h, &chn, RGBA_CHANNELS);
    if (data == NULL) {
        fprintf(stderr, "Failed to load image from file: %s\n", filepath);
        exit(EXIT_FAILURE);
    }

    return (image_data) {
        .width = w,
        .height = h,
        .num_channels = chn,
        .data = data
    };
}

sg_image create_sg_image_from_data(image_data img_data) {
    return sg_make_image(&(sg_image_desc) {
        .data.subimage[0][0] = {
            .ptr = img_data.data,
            .size = img_data.width * img_data.height * RGBA_CHANNELS
        },
        .width = img_data.width,
        .height = img_data.height,
        .pixel_format = SG_PIXELFORMAT_RGBA8
    });
}

/**
 * Automate the process of loading an image from a file and creating an sg_image.
 * Also frees the image data after creating the sg_image.
 */
sg_image create_sg_image_from_file(const char* filepath) {
    image_data img_data = load_image_from_file(filepath);
    if (img_data.data == NULL) {
        fprintf(stderr, "Failed to load image data from file: %s\n", filepath);
        exit(EXIT_FAILURE);
    }
    
    sg_image img = create_sg_image_from_data(img_data);
    if (img.id == SG_INVALID_ID) {
        fprintf(stderr, "Failed to create sg_image from data for file: %s\n", filepath);
        stbi_image_free(img_data.data);
        exit(EXIT_FAILURE);
    }

    stbi_image_free(img_data.data);
    return img;
}