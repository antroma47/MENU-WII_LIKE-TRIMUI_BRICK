#include "banner.h"
#include "../theme/theme.h"
#include <SDL2/SDL_image.h>
#include <string.h>
#include <math.h>

#define ANIM_SPEED  10.0f

void banner_init(Banner *b) {
    memset(b, 0, sizeof(Banner));
    b->alpha   = 0.0f;
    b->slide_x = -30.0f;
    b->visible = false;
}

void banner_free(Banner *b) {
    if (b->bg_tex)   { SDL_DestroyTexture(b->bg_tex);   b->bg_tex   = NULL; }
    if (b->logo_tex) { SDL_DestroyTexture(b->logo_tex); b->logo_tex = NULL; }
}

void banner_load(Banner *b, SDL_Renderer *renderer, const Channel *ch) {
    if (!ch) return;

    /* Scarica precedente */
    banner_free(b);

    strncpy(b->label, ch->label, CHANNEL_LABEL_LEN - 1);

    /* Banner background */
    if (ch->banner_path[0]) {
        SDL_Surface *s = IMG_Load(ch->banner_path);
        if (s) {
            b->bg_tex = SDL_CreateTextureFromSurface(renderer, s);
            SDL_FreeSurface(s);
        }
    }

    /* Logo (icona riutilizzata come logo se non c'è asset dedicato) */
    if (ch->icon_tex) {
        /* Riusa la texture già caricata — non distruggerla */
        b->logo_tex = ch->icon_tex;
    }

    /* Reset animazione */
    b->alpha   = 0.0f;
    b->slide_x = -30.0f;
}

void banner_show(Banner *b) {
    b->visible = true;
    b->alpha   = 0.0f;
    b->slide_x = -30.0f;
}

void banner_hide(Banner *b) {
    b->visible = false;
    b->alpha   = 0.0f;
}

void banner_update(Banner *b, float dt) {
    if (!b->visible) return;

    float target_alpha   = 1.0f;
    float target_slide_x = 0.0f;

    b->alpha   += (target_alpha   - b->alpha)   * ANIM_SPEED * dt;
    b->slide_x += (target_slide_x - b->slide_x) * ANIM_SPEED * dt;

    if (fabsf(b->alpha   - target_alpha)   < 0.01f) b->alpha   = target_alpha;
    if (fabsf(b->slide_x - target_slide_x) < 0.1f)  b->slide_x = target_slide_x;
}

void banner_render(SDL_Renderer *renderer, const Banner *b,
                   const SDL_Rect *dst) {
    if (!b->visible || b->alpha < 0.01f) return;

    Uint8 a = (Uint8)(b->alpha * 255.0f);
    int   ox = (int)b->slide_x;

    SDL_Rect area = {
        dst->x + ox,
        dst->y,
        dst->w,
        dst->h
    };

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    /* Sfondo banner */
    if (b->bg_tex) {
        SDL_SetTextureAlphaMod(b->bg_tex, a);
        SDL_RenderCopy(renderer, b->bg_tex, NULL, &area);
    } else {
        /* Fallback colore tema */
        ThemeColors *col = theme_get_colors();
        SDL_SetRenderDrawColor(renderer,
            col->banner_bg.r,
            col->banner_bg.g,
            col->banner_bg.b, a);
        SDL_RenderFillRect(renderer, &area);
    }

    /* Logo in basso a sinistra */
    if (b->logo_tex) {
        SDL_Rect logo = {
            area.x + 12,
            area.y + area.h - 52,
            40, 40
        };
        SDL_SetTextureAlphaMod(b->logo_tex, a);
        SDL_RenderCopy(renderer, b->logo_tex, NULL, &logo);
    }

    /* Bordo highlight */
    ThemeColors *col = theme_get_colors();
    SDL_SetRenderDrawColor(renderer,
        col->accent.r,
        col->accent.g,
        col->accent.b, a);
    SDL_RenderDrawRect(renderer, &area);
}
