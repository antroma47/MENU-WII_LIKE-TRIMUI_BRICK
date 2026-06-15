#ifndef THEME_H
#define THEME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

/* Colori tema */
typedef struct {
    SDL_Color background;
    SDL_Color channel_bg;
    SDL_Color channel_selected;
    SDL_Color channel_border;
    SDL_Color text_primary;
    SDL_Color text_secondary;
    SDL_Color accent;
    SDL_Color overlay;
} ThemeColors;

/* Font disponibili */
typedef enum {
    FONT_SMALL = 0,
    FONT_MEDIUM,
    FONT_LARGE,
    FONT_COUNT
} FontSize;

/* Struttura tema completa */
typedef struct {
    char        name[64];
    ThemeColors colors;
    TTF_Font   *fonts[FONT_COUNT];
    SDL_Texture *bg_texture;     /* sfondo opzionale */
    char        sounds_dir[256]; /* cartella suoni tema */
    bool        loaded;
} Theme;

/* API pubblica */
void  theme_init(SDL_Renderer *renderer);
void  theme_shutdown(void);
bool  theme_load(SDL_Renderer *renderer, const char *theme_dir);
void  theme_render_background(SDL_Renderer *renderer);

TTF_Font    *theme_get_font(FontSize size);
ThemeColors *theme_get_colors(void);

#endif /* THEME_H */
