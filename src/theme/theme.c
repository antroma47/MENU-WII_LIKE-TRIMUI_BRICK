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
        else if (!strcmp(key, "overlay"))          parse_color(val, &c->overlay);
        else if (!strcmp(key, "name"))             strncpy(g_theme.name, val, sizeof(g_theme.name));
        else if (!strcmp(key, "sounds_dir"))       strncpy(g_theme.sounds_dir, val, sizeof(g_theme.sounds_dir));
    }

    fclose(f);
}

/* ------------------------------------------------------------------ */
/* Font sizes in pixel — adattate allo schermo TrimUI Brick           */
/* ------------------------------------------------------------------ */
static const int FONT_SIZES[FONT_COUNT] = {
    10,  /* FONT_SMALL  */
    14,  /* FONT_MEDIUM */
    20   /* FONT_LARGE  */
};

static bool load_fonts(const char *theme_dir) {
    char font_path[512];

    /* Prova prima il font del tema, poi fallback interno */
    snprintf(font_path, sizeof(font_path), "%s/font.ttf", theme_dir);

    /* Verifica esistenza */
    FILE *test = fopen(font_path, "rb");
    if (!test) {
        /* Fallback al font di sistema */
        snprintf(font_path, sizeof(font_path), "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    } else {
        fclose(test);
    }

    for (int i = 0; i < FONT_COUNT; i++) {
        if (g_theme.fonts[i]) {
            TTF_CloseFont(g_theme.fonts[i]);
            g_theme.fonts[i] = NULL;
        }
        g_theme.fonts[i] = TTF_OpenFont(font_path, FONT_SIZES[i]);
        if (!g_theme.fonts[i]) {
            fprintf(stderr, "[theme] Font non caricato (%d px): %s\n",
                    FONT_SIZES[i], TTF_GetError());
            return false;
        }
    }
    return true;
}

/* ------------------------------------------------------------------ */
/* API pubblica                                                         */
/* ------------------------------------------------------------------ */
void theme_init(SDL_Renderer *renderer) {
    (void)renderer;
    memset(&g_theme, 0, sizeof(g_theme));
    theme_set_defaults();

    /* Carica font di default */
    load_fonts("assets/themes/default");
}

void theme_shutdown(void) {
    for (int i = 0; i < FONT_COUNT; i++) {
        if (g_theme.fonts[i]) {
            TTF_CloseFont(g_theme.fonts[i]);
            g_theme.fonts[i] = NULL;
        }
    }
    if (g_theme.bg_texture) {
        SDL_DestroyTexture(g_theme.bg_texture);
        g_theme.bg_texture = NULL;
    }
    g_theme.loaded = false;
}

bool theme_load(SDL_Renderer *renderer, const char *theme_dir) {
    /* Reset ai default prima di caricare */
    theme_set_defaults();

    /* Leggi configurazione */
    char conf_path[512];
    snprintf(conf_path, sizeof(conf_path), "%s/theme.conf", theme_dir);
    theme_parse_conf(conf_path);

    /* Carica font */
    if (!load_fonts(theme_dir)) return false;

    /* Carica sfondo opzionale */
    char bg_path[512];
    snprintf(bg_path, sizeof(bg_path), "%s/background.png", theme_dir);
    FILE *bg_test = fopen(bg_path, "rb");
    if (bg_test) {
        fclose(bg_test);
        if (g_theme.bg_texture) SDL_DestroyTexture(g_theme.bg_texture);
        SDL_Surface *surf = IMG_Load(bg_path);
        if (surf) {
            g_theme.bg_texture = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
        }
    }

    g_theme.loaded = true;
    printf("[theme] Tema caricato: %s\n", g_theme.name);
    return true;
}

void theme_render_background(SDL_Renderer *renderer) {
    if (g_theme.bg_texture) {
        SDL_RenderCopy(renderer, g_theme.bg_texture, NULL, NULL);
    } else {
        /* Sfondo solido colore tema */
        SDL_Color *bg = &g_theme.colors.background;
        SDL_SetRenderDrawColor(renderer, bg->r, bg->g, bg->b, bg->a);
        SDL_RenderClear(renderer);
    }
}

TTF_Font *theme_get_font(FontSize size) {
    if (size < 0 || size >= FONT_COUNT) return g_theme.fonts[FONT_MEDIUM];
    return g_theme.fonts[size];
}

ThemeColors *theme_get_colors(void) {
    return &g_theme.colors;
}
