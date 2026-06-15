#ifndef CHANNEL_H
#define CHANNEL_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#define CHANNEL_MAX          64
#define CHANNEL_LABEL_LEN    48
#define CHANNEL_PATH_LEN    256

/* Tipo canale */
typedef enum {
    CH_TYPE_EMULATOR = 0,
    CH_TYPE_GAME,
    CH_TYPE_APP,
    CH_TYPE_SETTINGS,
    CH_TYPE_MEDIA,
    CH_TYPE_FOLDER,
    CH_TYPE_SEPARATOR,
    CH_TYPE_COUNT
} ChannelType;

/* Stato animazione canale */
typedef struct {
    float scale;        /* 1.0 = normale, >1.0 = ingrandito  */
    float alpha;        /* 0.0 - 1.0                          */
    float offset_y;     /* offset verticale per hover effect  */
    bool  animating;
} ChannelAnim;

/* Definizione canale */
typedef struct {
    int          id;
    ChannelType  type;
    char         label[CHANNEL_LABEL_LEN];
    char         icon_path[CHANNEL_PATH_LEN];
    char         banner_path[CHANNEL_PATH_LEN];
    char         exec_path[CHANNEL_PATH_LEN];   /* emulatore / app da avviare */
    char         rom_path[CHANNEL_PATH_LEN];    /* ROM opzionale              */
    SDL_Texture *icon_tex;
    SDL_Texture *banner_tex;
    ChannelAnim  anim;
    bool         favorite;
    bool         visible;
    int          page;      /* pagina di appartenenza (0-based) */
    int          slot;      /* slot nella pagina (0-based)      */
} Channel;

/* Griglia canali */
typedef struct {
    Channel  channels[CHANNEL_MAX];
    int      count;
    int      selected;      /* indice canale selezionato */
    int      current_page;
    int      total_pages;
} ChannelGrid;

/* API pubblica */
void    channel_grid_init(ChannelGrid *grid);
void    channel_grid_free(ChannelGrid *grid);
bool    channel_grid_load(ChannelGrid *grid, SDL_Renderer *renderer, const char *conf_path);
bool    channel_grid_save(const ChannelGrid *grid, const char *conf_path);

Channel *channel_get_selected(ChannelGrid *grid);
void    channel_move(ChannelGrid *grid, int dx, int dy);
void    channel_page_next(ChannelGrid *grid);
void    channel_page_prev(ChannelGrid *grid);

void    channel_load_textures(Channel *ch, SDL_Renderer *renderer);
void    channel_free_textures(Channel *ch);

void    channel_update_anim(ChannelGrid *grid, float dt);
void    channel_render(SDL_Renderer *renderer, const Channel *ch,
                       const SDL_Rect *dst, bool selected);

#endif /* CHANNEL_H */
