#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

typedef enum {
    SFX_MOVE = 0,
    SFX_NAVIGATE,
    SFX_CONFIRM,
    SFX_BACK,
    SFX_PAGE,
    SFX_ERROR,
    SFX_COUNT
} SfxId;

void audio_init(void);
void audio_shutdown(void);
bool audio_load_sfx(SfxId id, const char *path);
void audio_play_sfx(SfxId id);
void audio_load_music(const char *path);
void audio_play_music(void);
void audio_stop_music(void);
void audio_set_sfx_volume(int vol);
void audio_set_music_volume(int vol);

#endif
