#include "audio.h"
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string.h>

static Mix_Chunk *g_sfx[SFX_COUNT] = {0};
static Mix_Music *g_music           = NULL;
static bool       g_ready           = false;

void audio_init(void) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0) {
        fprintf(stderr, "[audio] Mix_OpenAudio fallito: %s\n", Mix_GetError());
        return;
    }
    g_ready = true;
}

void audio_shutdown(void) {
    if (!g_ready) return;
    for (int i = 0; i < SFX_COUNT; i++) {
        if (g_sfx[i]) {
            Mix_FreeChunk(g_sfx[i]);
            g_sfx[i] = NULL;
        }
    }
    if (g_music) {
        Mix_FreeMusic(g_music);
        g_music = NULL;
    }
    Mix_CloseAudio();
    g_ready = false;
}

bool audio_load_sfx(SfxId id, const char *path) {
    if (!g_ready || id < 0 || id >= SFX_COUNT) return false;
    if (g_sfx[id]) {
        Mix_FreeChunk(g_sfx[id]);
        g_sfx[id] = NULL;
    }
    g_sfx[id] = Mix_LoadWAV(path);
    if (!g_sfx[id]) {
        fprintf(stderr, "[audio] Impossibile caricare %s: %s\n", path, Mix_GetError());
        return false;
    }
    return true;
}

void audio_play_sfx(SfxId id) {
    if (!g_ready || id < 0 || id >= SFX_COUNT) return;
    if (!g_sfx[id]) return;
    Mix_PlayChannel(-1, g_sfx[id], 0);
}

void audio_load_music(const char *path) {
    if (!g_ready) return;
    if (g_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(g_music);
        g_music = NULL;
    }
    g_music = Mix_LoadMUS(path);
    if (!g_music) {
        fprintf(stderr, "[audio] Impossibile caricare musica %s: %s\n", path, Mix_GetError());
    }
}

void audio_play_music(void) {
    if (!g_ready || !g_music) return;
    if (!Mix_PlayingMusic()) {
        Mix_PlayMusic(g_music, -1);
    }
}

void audio_stop_music(void) {
    if (!g_ready) return;
    Mix_HaltMusic();
}

void audio_set_sfx_volume(int vol) {
    if (!g_ready) return;
    for (int i = 0; i < SFX_COUNT; i++) {
        if (g_sfx[i]) Mix_VolumeChunk(g_sfx[i], vol);
    }
}

void audio_set_music_volume(int vol) {
    if (!g_ready) return;
    Mix_VolumeMusic(vol);
}
