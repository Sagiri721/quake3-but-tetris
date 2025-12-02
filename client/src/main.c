#include "core/rng.h"
#define SOKOL_IMPL
#if defined (_WIN32)
    #define SOKOL_D3D11
#elif defined (__APPLE__)
    #define SOKOL_METAL
#else
    #define SOKOL_GLCORE33
#endif

#define SOKOL_IMPL
#include "sokol_gp/thirdparty/sokol_gfx.h"
#include "sokol_gp/sokol_gp.h"
#include "sokol_gp/thirdparty/sokol_app.h"
#include "sokol_gp/thirdparty/sokol_glue.h"
#include "sokol_gp/thirdparty/sokol_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "game.h"

#define TARGET_WINDOW_WIDTH 1280
#define TARGET_WINDOW_HEIGHT 720

// Called on every frame of the application.
static void frame(void) {
    update_game();
}

// Called when the application is initializing.
static void init(void) {
    // Initialize Sokol GFX.
    sg_desc sgdesc = {
        .environment = sglue_environment(),
        .logger.func = slog_func
    };
    sg_setup(&sgdesc);

    if (!sg_isvalid()) {
        fprintf(stderr, "Failed to create Sokol GFX context!\n");
        exit(EXIT_FAILURE);
    }

    // Initialize Sokol GP, adjust the size of command buffers for your own use.
    sgp_desc sgpdesc = {0};
    sgp_setup(&sgpdesc);
    if (!sgp_is_valid()) {
        fprintf(stderr, "Failed to create Sokol GP context: %s\n", sgp_get_error_message(sgp_get_last_error()));
        exit(EXIT_FAILURE);
    }

    setup_game();
}

static void input(const sapp_event* event) {
    //printf("Event type: %d\n", event->type);
    event_game(event);
}

// Called when the application is shutting down.
static void cleanup(void) {

    cleanup_game();

    // Cleanup Sokol GP and Sokol GFX resources.
    sgp_shutdown();
    sg_shutdown();
}

// Implement application main through Sokol APP.
sapp_desc sokol_main(int argc, char* argv[]) {

    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = input,
        .width = TARGET_WINDOW_WIDTH,
        .height = TARGET_WINDOW_HEIGHT,
        .window_title = "Rectangle (Sokol GP)",
        .logger.func = slog_func,
    };
}