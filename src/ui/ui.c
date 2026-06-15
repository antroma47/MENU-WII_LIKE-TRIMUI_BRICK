#include "ui.h"
#include "../theme/theme.h"
#include "../audio/audio.h"
#include "../channels/channel.h"
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* Layout costanti (CHANNEL_PAD viene da theme.h)                      */
/* ------------------------------------------------------------------ */
#define BANNER_H_RATIO   0.28f   /* banner occupa 28% altezza screen  */
#define GRID_TOP_RATIO   0.30f   /* griglia parte al 30%              */
#define WIDGET_PAD       6

/* ------------------------------------------------------------------ */
/* Precalcola geometrie slot griglia                                    */
/* 4 colonne x 2 righe                                                 */
/* ------------------------------------------------------------------ */
static void compute_slot_rects(UI *ui) {
    int grid_top  = (int)(ui->screen_h * GRID_TOP_RATIO);
    int grid_h    = ui->screen_h - grid_top - 20;
    int cell_w    = (ui->screen_w - CHANNEL_PAD * (COLS_PER_PAGE + 1)) / COLS_PER_PAGE;
    int cell_h    = (grid_h      - CHANNEL_PAD * (ROWS_PER_PAGE + 1)) / ROWS_PER_PAGE;

    for (int row = 0; row < ROWS_PER_PAGE; row++) {
        for (int col = 0; col < COLS_PER_PAGE; col++) {
            int idx = row * COLS_PER_PAGE + col;
            ui->slot_rects[idx].x = CHANNEL_PAD + col * (cell_w + CHANNEL_PAD);
            ui->slot_rects[idx].y = grid_top + CHANNEL_PAD + row * (cell_h + CHANNEL_PAD);
            ui->slot_rects[idx].w = cell_w;
            ui->slot_rects[idx].h = cell_h;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Init                                                                 */
/* ------------------------------------------------------------------ */
bool ui_init(UI *ui, SDL_Renderer *renderer, int w, int h) {
    memset(ui, 0, sizeof(UI));

    ui->renderer  = renderer;
    ui->screen_w  = w;
    ui->screen_h  = h;
    ui->state     = UI_STATE_HOME;
    ui->show_clock   = true;
    ui->show_battery = true;

    /* Font */
    ui->font_large = TTF_OpenFont("assets/fonts/default.ttf", 16);
    ui->font_small = TTF_OpenFont("assets/fonts/default.ttf", 11);
    if (!ui->font_large || !ui->font_small) {
        fprintf(stderr, "ui: font non trovato\n");
        return false;
    }

    /* Griglia canali */
    channel_grid_init(&ui->grid);
    if (!channel_grid_load(&ui->grid, renderer, "config/channels.conf")) {
        fprintf(stderr, "ui: warning - channels.conf non trovato\n");
    }

    /* Banner */
    banner_init(&ui->banner);
    Channel *sel = channel_get_selected(&ui->grid);
    if (sel) {
        banner_load(&ui->banner, renderer, sel);
        banner_show(&ui->banner);
    }

    /* Layout */
    compute_slot_rects(ui);

    /* Orologio iniziale */
    ui->last_clock_update = 0;
    strncpy(ui->clock_str, "00:00", sizeof(ui->clock_str) - 1);

    return true;
}

/* ------------------------------------------------------------------ */
/* Free                                                                 */
/* ------------------------------------------------------------------ */
void ui_free(UI *ui) {
    channel_grid_free(&ui->grid);
    banner_free(&ui->banner);
    if (ui->font_large) TTF_CloseFont(ui->font_large);
    if (ui->font_small) TTF_CloseFont(ui->font_small);
    memset(ui, 0, sizeof(UI));
}

/* ------------------------------------------------------------------ */
/* Input                                                                */
/* ------------------------------------------------------------------ */
void ui_handle_input(UI *ui, SDL_Keycode key) {
    if (ui->transitioning) return;

    switch (ui->state) {

    case UI_STATE_HOME:
        switch (key) {
        case SDLK_RIGHT:
            channel_move(&ui->grid, 1, 0);
            audio_play_sfx(SFX_NAVIGATE);
            break;
        case SDLK_LEFT:
            channel_move(&ui->grid, -1, 0);
            audio_play_sfx(SFX_NAVIGATE);
            break;
        case SDLK_DOWN:
            channel_move(&ui->grid, 0, 1);
            audio_play_sfx(SFX_NAVIGATE);
            break;
        case SDLK_UP:
            channel_move(&ui->grid, 0, -1);
            audio_play_sfx(SFX_NAVIGATE);
            break;
        case SDLK_PERIOD:   /* L trigger -> pagina precedente */
            channel_page_prev(&ui->grid);
            ui->page_trans       = PAGE_TRANS_RIGHT;
            ui->page_trans_x     = (float)ui->screen_w;
            ui->transitioning    = true;
            audio_play_sfx(SFX_PAGE);
            break;
        case SDLK_SLASH:    /* R trigger -> pagina successiva */
            channel_page_next(&ui->grid);
            ui->page_trans       = PAGE_TRANS_LEFT;
            ui->page_trans_x     = -(float)ui->screen_w;
            ui->transitioning    = true;
            audio_play_sfx(SFX_PAGE);
            break;
        case SDLK_RETURN:   /* A / conferma */
            ui->state = UI_STATE_CHANNEL_OPEN;
            audio_play_sfx(SFX_CONFIRM);
            break;
        default: break;
        }

        /* Aggiorna banner al nuovo selezionato */
        {
            Channel *sel = channel_get_selected(&ui->grid);
            if (sel) {
                banner_load(&ui->banner, ui->renderer, sel);
                banner_show(&ui->banner);
            }
        }
        break;

    case UI_STATE_CHANNEL_OPEN:
        if (key == SDLK_ESCAPE || key == SDLK_BACKSPACE) {
            ui->state = UI_STATE_HOME;
            audio_play_sfx(SFX_BACK);
        }
        break;

    default: break;
    }
}

/* ------------------------------------------------------------------ */
/* Update                                                               */
/* ------------------------------------------------------------------ */
static void update_clock(UI *ui) {
    Uint32 now = SDL_GetTicks();
    if (now - ui->last_clock_update < 15000) return;  /* aggiorna ogni 15s */
    ui->last_clock_update = now;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(ui->clock_str, sizeof(ui->clock_str), "%H:%M", tm_info);
}

static void update_page_transition(UI *ui, float dt) {
    if (!ui->transitioning) return;

    float speed = ui->screen_w * 6.0f;   /* pixel/sec */

    if (ui->page_trans == PAGE_TRANS_LEFT) {
        ui->page_trans_x += speed * dt;
        if (ui->page_trans_x >= 0.0f) {
            ui->page_trans_x  = 0.0f;
            ui->transitioning = false;
        }
    } else {
        ui->page_trans_x -= speed * dt;
        if (ui->page_trans_x <= 0.0f) {
            ui->page_trans_x  = 0.0f;
            ui->transitioning = false;
        }
    }
}

void ui_update(UI *ui, float dt) {
    update_clock(ui);
    update_page_transition(ui, dt);
    channel_update_anim(&ui->grid, dt);
    banner_update(&ui->banner, dt);
}

/* ------------------------------------------------------------------ */
/* Helper: render testo semplice                                        */
/* ------------------------------------------------------------------ */
static void render_text(UI *ui, TTF_Font *font,
                        const char *text, SDL_Color col,
                        int x, int y) {
    if (!text || !text[0]) return;
    SDL_Surface *s = TTF_RenderUTF8_Blended(font, text, col);
    if (!s) return;
    SDL_Texture *t = SDL_CreateTextureFromSurface(ui->renderer, s);
    SDL_Rect dst = { x, y, s->w, s->h };
    SDL_FreeSurface(s);
    if (!t) return;
    SDL_RenderCopy(ui->renderer, t, NULL, &dst);
    SDL_DestroyTexture(t);
}

/* ------------------------------------------------------------------ */
/* Render widget (clock + battery)                                      */
/* ------------------------------------------------------------------ */
static void render_widgets(UI *ui) {
    ThemeColors *col = theme_get_colors();
    SDL_Color    tc  = col->text;

    if (ui->show_clock) {
        render_text(ui, ui->font_small, ui->clock_str, tc,
                    ui->screen_w - 48, WIDGET_PAD);
    }

    if (ui->show_battery) {
        render_text(ui, ui->font_small, ui->battery_str, tc,
                    ui->screen_w - 96, WIDGET_PAD);
    }
}

/* ------------------------------------------------------------------ */
/* Render indicatori pagina (pallini stile Wii)                         */
/* ------------------------------------------------------------------ */
static void render_page_dots(UI *ui) {
    if (ui->grid.total_pages <= 1) return;

    ThemeColors *col    = theme_get_colors();
    int          total  = ui->grid.total_pages;
    int          dot_r  = 4;
    int          gap    = 12;
    int          total_w = total * (dot_r * 2) + (total - 1) * (gap - dot_r * 2);
    int          sx     = (ui->screen_w - total_w) / 2;
    int          sy     = ui->screen_h - 12;

    for (int i = 0; i < total; i++) {
        SDL_Color c = (i == ui->grid.current_page) ? col->accent : col->text;
        SDL_SetRenderDrawColor(ui->renderer, c.r, c.g, c.b, 180);
        SDL_Rect dot = {
            sx + i * gap - dot_r,
            sy - dot_r,
            dot_r * 2, dot_r * 2
        };
        SDL_RenderFillRect(ui->renderer, &dot);
    }
}

/* ------------------------------------------------------------------ */
/* Render griglia canali                                                */
/* ------------------------------------------------------------------ */
static void render_grid(UI *ui) {
    int offset_x = (int)ui->page_trans_x;

    for (int i = 0; i < ui->grid.count; i++) {
        Channel *ch = &ui->grid.channels[i];
        if (ch->page != ui->grid.current_page) continue;
        if (ch->slot < 0 || ch->slot >= SLOTS_PER_PAGE) continue;

        SDL_Rect dst = ui->slot_rects[ch->slot];
        dst.x += offset_x;

        bool sel = (i == ui->grid.selected);
        channel_render(ui->renderer, ch, &dst, sel);

        /* Label sotto il canale */
        if (sel) {
            ThemeColors *col = theme_get_colors();
            render_text(ui, ui->font_small, ch->label, col->text,
                        dst.x + 4, dst.y + dst.h + 2);
        }
    }
}

/* ------------------------------------------------------------------ */
/* Render principale                                                    */
/* ------------------------------------------------------------------ */
void ui_render(UI *ui) {
    ThemeColors *col = theme_get_colors();

    /* Sfondo */
    SDL_SetRenderDrawColor(ui->renderer,
        col->background.r,
        col->background.g,
        col->background.b, 255);
    SDL_RenderClear(ui->renderer);

    /* Banner area in alto */
    SDL_Rect banner_rect = {
        0, 0,
        ui->screen_w,
        (int)(ui->screen_h * BANNER_H_RATIO)
    };
    banner_render(ui->renderer, &ui->banner, &banner_rect);

    /* Griglia */
    render_grid(ui);

    /* Pallini pagina */
    render_page_dots(ui);

    /* Widget */
    render_widgets(ui);

    SDL_RenderPresent(ui->renderer);
}
