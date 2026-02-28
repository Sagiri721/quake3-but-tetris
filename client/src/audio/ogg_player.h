/**
 * @file        ogg_player.h
 * @brief       Ogg file reading and streaming
 */

#ifndef OGG_PLAYER
#define OGG_PLAYER

// Implementation based on https://github.com/vlang/v/blob/master/examples/sokol/sounds/ogg_player.v

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c" // I DIDNT KNOW YOU COULD DO THIS !????

#include "../queue/queue.h"
#include "../lib/tinycthread.h"

typedef struct {
    int channels;
    unsigned int sample_rate;

    int pos;
    char finished;
    char loop;
    float volume;

    int push_slack_ms;

    unsigned int stream_rate;
    int stream_channels;
    unsigned int stream_len_samples;
    float stream_len_seconds;

    stb_vorbis_alloc* alloc;
    stb_vorbis* vorbis;

    // The thread that pushes audio
    thrd_t audio_thread;
    mtx_t mutex;
    cnd_t cond;

    volatile char should_stop;
    char running;

} ogg_audio_player;

typedef enum {
    SFX_MOVE_PIECE, SFX_ROTATE_PIECE, SFX_LINE_CLEAR, SFX_LEVEL_UP, SFX_GAME_OVER, SFX_PIECE_LANDED
} ogg_sfx;

typedef struct {
    ogg_sfx type;
    int channels;
    unsigned int sample_rate;

    int pos;
    char active;

    unsigned int num_samples;
    float* samples;

} ogg_sound;
extern char* ogg_sfx_paths[];

#define SFX_POOL_SIZE 16

typedef struct {
    // The sounds effects in the queue
    queue sfx_queue;
    mtx_t mutex;

    ogg_sound sfx_pool[SFX_POOL_SIZE];
    size_t pool_top;

} ogg_sfx_queue;

extern ogg_sfx_queue global_sfx_queue;

void audio_init(ogg_audio_player* player, int push_slack_ms, char loop);
void audio_destroy(ogg_audio_player* player);

void audio_set_volume(ogg_audio_player* player, float volume);

void push_ogg_file(ogg_audio_player* player, const char* path);

void sfx_queue_init(ogg_sfx_queue* queue);
void sfx_queue_destroy(ogg_sfx_queue* queue);
int sfx_enqueue(ogg_sfx_queue* queue, int sfx);

#endif