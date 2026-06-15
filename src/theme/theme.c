#include "theme.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

/* Tema attivo globale */
static Theme g_theme = {0};

/* ------------------------------------------------------------------ */
/* Tema di default ispirato al Wii Menu (bianco/blu)                  */
/* ------------------------------------------------------------------ */
static void theme_set_defaults(void) {
    strncpy(g_theme.name, "default", sizeof(g_theme.name));

    g_theme.colors.background       = (SDL_Color){240, 244, 255, 255};
    g_theme.colors.channel_bg       = (SDL_Color){255, 255, 255, 220};
    g_theme.colors.channel_selected = (SDL_Color){100, 180, 255, 255};
    g_theme.colors.channel_border   = (SDL_Color){180, 200, 230, 255};
    g_theme.colors.text_primary     = (SDL_Color){ 30,  30,  30, 255};
    g_theme.colors.text_secondary   = (SDL_Color){100, 100, 120, 255};
    g_theme.colors.accent           = (SDL_Color){ 50, 140, 255, 255};
    g_theme.colors.overlay          = (SDL_Color){  0,   0,   0, 120};

    g_theme.bg_texture = NULL;
    g_theme.sounds_dir[0] = '\0';
    g_theme.loaded = false;
}

/* ------------------------------------------------------------------ */
/* Parsing minimo di theme.conf                                        */
/* Formato: chiave=valore, un per riga                                 */
/* Esempio: background=240,244,255                                     */
/* ------------------------------------------------------------------ */
static void parse_color(const char *val, SDL_Color *out) {
    int r = 255, g = 255, b = 255, a = 255;
    sscanf(val, "%d,%d,%d,%d", &r, &g, &b, &a);
    out->r = (Uint8)r;
    out->g = (Uint8)g;
    out->b = (Uint8)b;
    out->a = (Uint8)a;
}

static void theme_parse_conf(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        /* Salta commenti e righe vuote */
        if (line[0] == '#' || line[0] == '\n') continue;

        char key[64], val[128];
        if (sscanf(line, "%63[^=]=%127[^\n]", key, val) != 2) continue;

        ThemeColors *c = &g_theme.colors;
        if      (!strcmp(key, "background"))       parse_color(val, &c->background);
        else if (!strcmp(key, "channel_bg"))       parse_color(val, &c->channel_bg);
        else if (!strcmp(key, "channel_selected")) parse_color(val, &c->channel_selected);
        else if (!strcmp(key, "channel_border"))   parse_color(val, &c->channel_border);
        else if (!strcmp(key, "text_primary"))     parse_color(val, &c->text_primary);
        else if (!strcmp(key, "text_secondary"))   parse_color(val, &c->text_secondary);
        else if (!strcmp(key, "accent"))           parse_color(val, &c->accent);
        else if (!strcmp(key, "overlay"))          parse_color(
