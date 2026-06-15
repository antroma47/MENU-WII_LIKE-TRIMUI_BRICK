#ifndef BANNER_H
#define BANNER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "../channels/channel.h"

typedef struct {
    SDL_Texture *bg_tex;
    SDL_Texture *logo_tex;
    char         label[CHANNEL_LABEL_LEN];
    float        alpha;
    float        slide_x;
    bool         visible;
    int          target_w;
    int          target_h;
} Banner;

void banner_init(SDL_Renderer *renderer);
void banner_free(Banner *b);
void banner_load(Banner *b, SDL_Renderer *renderer, const Channel *ch);
void banner_show(Banner *b);
void banner_hide(Banner *b);
void banner_update(void);
void banner_render(SDL_Renderer *renderer);
void banner_shutdown(void);

#endif
