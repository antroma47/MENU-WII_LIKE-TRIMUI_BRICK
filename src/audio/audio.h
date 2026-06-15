#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

typedef enum {
    SFX_NAVIGATE = 0, /* alias per SFX_MOVE (compatibilità ui.c) */
    SFX_MOVE = 0,     /* navigazione canali    */
    SFX_CONFIRM,      /* conferma / avvio      */
    SFX_BACK,         /* tasto B / indietro    */
    SFX_PAGE,         /* cambio pagina L/R     */
    SFX_ERROR,        /* azione non valida     */
    SFX_COUNT
} SfxId;

/* API pubblica */
void audio_init(void);
void audio_shutdown(void);
bool audio_load_sfx(SfxId id, const char *path);
void audio_play_sfx(SfxId id);
void audio_load_music(const char *path);
void audio_play_music(void);
void audio_stop_music(void);
void audio_set_sfx_volume(int vol);    /* 0-128 */
void audio_set_music_volume(int vol);  /* 0-128 */

#endif /* AUDIO_H */
