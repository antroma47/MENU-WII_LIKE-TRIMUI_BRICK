#ifndef BANNER_H
#define BANNER_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "../channels/channel.h"

typedef struct {
    SDL_Texture *bg_tex;        /* sfondo banner                  */
    SDL_Texture *logo_tex;      /* logo canale                    */
    char         label[CHANNEL_LABEL_LEN];
    float        alpha;         /* fade in/out 0.0-1.0            */
    float        slide_x;      /* slide animazione (-1.0 -> 0.0) */
    bool         visible;
    int          target_w;
    int          target_h;
} Banner;

void banner_init(Banner *b);
void banner_free(Banner *b);
void banner_load(Banner *b, SDL_Renderer *renderer, const Channel *ch);
void banner_show(Banner *b);
void banner_hide(Banner *b);
void banner_update(Banner *b, float dt);
void banner_render(SDL_Renderer *renderer, const Banner *b,
                   const SDL_Rect *dst);

#endif /* BANNER_H */
