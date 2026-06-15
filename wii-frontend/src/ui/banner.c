#include "banner.h"
#include "../theme/theme.h"
#include <SDL2/SDL_image.h>
#include <string.h>
#include <math.h>

#define ANIM_SPEED  10.0f

static Banner g_banner = {0};
static SDL_Renderer *g_renderer = NULL;

void banner_init(SDL_Renderer *renderer) {
    g_renderer = renderer;
    memset(&g_banner, 0, sizeof(Banner));
    g_banner.alpha   = 0.0f;
    g_banner.slide_x = -30.0f;
    g_banner.visible = false;
}

void banner_free(Banner *b) {
    if (b->bg_tex)   { SDL_DestroyTexture(b->bg_tex);   b->bg_tex   = NULL; }
    if (b->logo_tex) { SDL_DestroyTexture(b->logo_tex); b->logo_tex = NULL; }
}

void banner_load(Banner *b, SDL_Renderer *renderer, const Channel *ch) {
    if (!ch) return;
    banner_free(b);
    strncpy(b->label, ch->label, CHANNEL_LABEL_LEN - 1);
    if (ch->banner_path[0]) {
        SDL_Surface *s = IMG_Load(ch->banner_path);
        if (s) {
            b->bg_tex = SDL_CreateTextureFromSurface(renderer, s);
            SDL_FreeSurface(s);
        }
    }
    if (ch->icon_tex) {
        b->logo_tex = ch->icon_tex;
    }
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

void banner_update(void) {
    if (!g_banner.visible) return;
    float target_alpha   = 1.0f;
    float target_slide_x = 0.0f;
    g_banner.alpha   += (target_alpha   - g_banner.alpha)   * ANIM_SPEED * 0.016f;
    g_banner.slide_x += (target_slide_x - g_banner.slide_x) * ANIM_SPEED * 0.016f;
    if (fabsf(g_banner.alpha   - target_alpha)   < 0.01f) g_banner.alpha   = target_alpha;
    if (fabsf(g_banner.slide_x - target_slide_x) < 0.1f)  g_banner.slide_x = target_slide_x;
}

void banner_render(SDL_Renderer *renderer) {
    if (!g_banner.visible || g_banner.alpha < 0.01f) return;
    Uint8 a = (Uint8)(g_banner.alpha * 255.0f);
    int   ox = (int)g_banner.slide_x;
    SDL_Rect area = { ox, 0, 1024, 256 };
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    if (g_banner.bg_tex) {
        SDL_SetTextureAlphaMod(g_banner.bg_tex, a);
        SDL_RenderCopy(renderer, g_banner.bg_tex, NULL, &area);
    } else {
        ThemeColors *col = theme_get_colors();
        SDL_SetRenderDrawColor(renderer,
            col->channel_bg.r,
            col->channel_bg.g,
            col->channel_bg.b, a);
        SDL_RenderFillRect(renderer, &area);
    }
    if (g_banner.logo_tex) {
        SDL_Rect logo = {
            area.x + 12,
            area.y + area.h - 52,
            40, 40
        };
        SDL_SetTextureAlphaMod(g_banner.logo_tex, a);
        SDL_RenderCopy(renderer, g_banner.logo_tex, NULL, &logo);
    }
}

void banner_shutdown(void) {
    banner_free(&g_banner);
    g_renderer = NULL;
}
