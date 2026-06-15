#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>  /* ← AGGIUNGI QUESTO */
#include "banner.h"

#define SLOTS_PER_PAGE 8   /* 8 canali per pagina */
#define COLS_PER_PAGE 4    /* 4 colonne */
#define ROWS_PER_PAGE 2    /* 2 righe */
#define MAX_PAGES 10
#define MAX_CHANNELS (SLOTS_PER_PAGE * MAX_PAGES)


/* resto del file... */


/* Stato generale UI */
typedef enum {
    UI_STATE_HOME = 0,
    UI_STATE_CHANNEL_OPEN,
    UI_STATE_GAME_LIST,
    UI_STATE_SETTINGS,
    UI_STATE_COUNT
} UIState;

/* Transizione pagina */
typedef enum {
    PAGE_TRANS_NONE = 0,
    PAGE_TRANS_LEFT,
    PAGE_TRANS_RIGHT
} PageTransition;

typedef struct {
    SDL_Renderer   *renderer;
    TTF_Font       *font_large;
    TTF_Font       *font_small;

    ChannelGrid     grid;
    Banner          banner;

    UIState         state;

    /* Transizione pagina */
    PageTransition  page_trans;
    float           page_trans_x;   /* 0.0 = completata           */
    bool            transitioning;

    /* Layout */
    int             screen_w;
    int             screen_h;

    /* Slot geometrie (precalcolate) */
    SDL_Rect        slot_rects[SLOTS_PER_PAGE];  /* 8 slot per pagina */

    /* Widget */
    bool            show_clock;
    bool            show_battery;

    /* Timer per orologio */
    Uint32          last_clock_update;
    char            clock_str[16];
    char            battery_str[8];
} UI;

/* API */
bool ui_init(UI *ui, SDL_Renderer *renderer, int w, int h);
void ui_free(UI *ui);
void ui_handle_input(UI *ui, SDL_Keycode key);
void ui_update(UI *ui, float dt);
void ui_render(UI *ui);

#endif /* UI_H */
