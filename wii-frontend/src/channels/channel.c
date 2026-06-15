#include "channel.h"
#include "../theme/theme.h"
#include "../audio/audio.h"
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static ChannelGrid g_grid = {0};

void channel_grid_init(ChannelGrid *grid) {
    memset(grid, 0, sizeof(ChannelGrid));
    grid->selected     = 0;
    grid->current_page = 0;
    grid->total_pages  = 1;
}

void channel_grid_free(ChannelGrid *grid) {
    for (int i = 0; i < grid->count; i++) {
        channel_free_textures(&grid->channels[i]);
    }
    memset(grid, 0, sizeof(ChannelGrid));
}

static ChannelType parse_type(const char *s) {
    if (strcmp(s, "emulator") == 0) return CH_TYPE_EMULATOR;
    if (strcmp(s, "game")     == 0) return CH_TYPE_GAME;
    if (strcmp(s, "app")      == 0) return CH_TYPE_APP;
    if (strcmp(s, "settings") == 0) return CH_TYPE_SETTINGS;
    if (strcmp(s, "media")    == 0) return CH_TYPE_MEDIA;
    if (strcmp(s, "folder")   == 0) return CH_TYPE_FOLDER;
    return CH_TYPE_APP;
}

bool channel_grid_load(ChannelGrid *grid, SDL_Renderer *renderer,
                       const char *conf_path) {
    FILE *f = fopen(conf_path, "r");
    if (!f) {
        fprintf(stderr, "[channel] Impossibile aprire %s\n", conf_path);
        return false;
    }

    channel_grid_init(grid);

    char   line[512];
    Channel *cur = NULL;
    int     max_page = 0;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "[channel]") == 0) {
            if (grid->count >= CHANNEL_MAX) break;
            cur = &grid->channels[grid->count++];
            memset(cur, 0, sizeof(Channel));
            cur->id      = grid->count - 1;
            cur->visible = true;
            cur->anim.scale   = 1.0f;
            cur->anim.alpha   = 1.0f;
            cur->anim.offset_y = 0.0f;
            continue;
        }
        if (!cur) continue;
        char key[64], val[448];
        if (sscanf(line, "%63[^=]=%447[^\n]", key, val) != 2) continue;
        if      (strcmp(key, "label")  == 0) strncpy(cur->label,       val, CHANNEL_LABEL_LEN  - 1);
        else if (strcmp(key, "type")   == 0) cur->type = parse_type(val);
        else if (strcmp(key, "icon")   == 0) strncpy(cur->icon_path,   val, CHANNEL_PATH_LEN   - 1);
        else if (strcmp(key, "banner") == 0) strncpy(cur->banner_path, val, CHANNEL_PATH_LEN   - 1);
        else if (strcmp(key, "exec")   == 0) strncpy(cur->exec_path,   val, CHANNEL_PATH_LEN   - 1);
        else if (strcmp(key, "rom")    == 0) strncpy(cur->rom_path,    val, CHANNEL_PATH_LEN   - 1);
        else if (strcmp(key, "page")   == 0) { cur->page = atoi(val); if (cur->page > max_page) max_page = cur->page; }
        else if (strcmp(key, "slot")   == 0) cur->slot = atoi(val);
        else if (strcmp(key, "favorite") == 0) cur->favorite = (atoi(val) == 1);
    }
    fclose(f);
    grid->total_pages = max_page + 1;
    for (int i = 0; i < grid->count; i++) {
        if (grid->channels[i].page == grid->current_page) {
            channel_load_textures(&grid->channels[i], renderer);
        }
    }
    printf("[channel] Caricati %d canali, %d pagine\n", grid->count, grid->total_pages);
    return true;
}

bool channel_grid_save(const ChannelGrid *grid, const char *conf_path) {
    FILE *f = fopen(conf_path, "w");
    if (!f) return false;
    for (int i = 0; i < grid->count; i++) {
        const Channel *ch = &grid->channels[i];
        fprintf(f,
            "[channel]\nlabel=%s\ntype=%d\nicon=%s\nbanner=%s\nexec=%s\nrom=%s\npage=%d\nslot=%d\nfavorite=%d\n\n",
            ch->label, ch->type,
            ch->icon_path, ch->banner_path,
            ch->exec_path, ch->rom_path,
            ch->page, ch->slot,
            ch->favorite ? 1 : 0);
    }
    fclose(f);
    return true;
}

Channel *channel_get_selected(ChannelGrid *grid) {
    if (grid->count == 0) return NULL;
    return &grid->channels[grid->selected];
}

static int find_channel(const ChannelGrid *grid, int page, int slot) {
    for (int i = 0; i < grid->count; i++) {
        if (grid->channels[i].page == page &&
            grid->channels[i].slot == slot &&
            grid->channels[i].visible) {
            return i;
        }
    }
    return -1;
}

void channel_move(ChannelGrid *grid, int dx, int dy) {
    if (grid->count == 0) return;
    Channel *cur = channel_get_selected(grid);
    if (!cur) return;
    int new_slot = cur->slot + dx + dy * COLS_PER_PAGE;
    int page     = cur->page;
    if (new_slot < 0) {
        channel_page_prev(grid);
        return;
    }
    if (new_slot >= SLOTS_PER_PAGE) {
        channel_page_next(grid);
        return;
    }
    int idx = find_channel(grid, page, new_slot);
    if (idx >= 0) {
        grid->selected = idx;
    }
}

static void switch_page(ChannelGrid *grid, int new_page, SDL_Renderer *renderer) {
    if (new_page < 0 || new_page >= grid->total_pages) return;
    for (int i = 0; i < grid->count; i++) {
        if (grid->channels[i].page == grid->current_page) {
            channel_free_textures(&grid->channels[i]);
        }
    }
    grid->current_page = new_page;
    for (int i = 0; i < grid->count; i++) {
        if (grid->channels[i].page == new_page) {
            channel_load_textures(&grid->channels[i], renderer);
        }
    }
    int idx = find_channel(grid, new_page, 0);
    if (idx >= 0) grid->selected = idx;
}

void channel_page_next(ChannelGrid *grid) {
    switch_page(grid, grid->current_page + 1, NULL);
}

void channel_page_prev(ChannelGrid *grid) {
    switch_page(grid, grid->current_page - 1, NULL);
}

void channel_load_textures(Channel *ch, SDL_Renderer *renderer) {
    if (!renderer) return;
    if (ch->icon_path[0] && !ch->icon_tex) {
        SDL_Surface *s = IMG_Load(ch->icon_path);
        if (s) {
            ch->icon_tex = SDL_CreateTextureFromSurface(renderer, s);
            SDL_FreeSurface(s);
        }
    }
    if (ch->banner_path[0] && !ch->banner_tex) {
        SDL_Surface *s = IMG_Load(ch->banner_path);
        if (s) {
            ch->banner_tex = SDL_CreateTextureFromSurface(renderer, s);
            SDL_FreeSurface(s);
        }
    }
}

void channel_free_textures(Channel *ch) {
    if (ch->icon_tex)   { SDL_DestroyTexture(ch->icon_tex);   ch->icon_tex   = NULL; }
    if (ch->banner_tex) { SDL_DestroyTexture(ch->banner_tex); ch->banner_tex = NULL; }
}

#define ANIM_SPEED       8.0f
#define SCALE_SELECTED   1.06f
#define OFFSET_SELECTED -3.0f

void channel_update_anim(ChannelGrid *grid, float dt) {
    for (int i = 0; i < grid->count; i++) {
        if (grid->channels[i].page != grid->current_page) continue;
        Channel     *ch  = &grid->channels[i];
        ChannelAnim *an  = &ch->anim;
        bool         sel = (i == grid->selected);
        float target_scale  = sel ? SCALE_SELECTED : 1.0f;
        float target_offset = sel ? OFFSET_SELECTED : 0.0f;
        an->scale    += (target_scale  - an->scale)    * ANIM_SPEED * dt;
        an->offset_y += (target_offset - an->offset_y) * ANIM_SPEED * dt;
        if (fabsf(an->scale    - target_scale)  < 0.001f) an->scale    = target_scale;
        if (fabsf(an->offset_y - target_offset) < 0.1f)   an->offset_y = target_offset;
    }
}

void channel_render(SDL_Renderer *renderer, const Channel *ch,
                    const SDL_Rect *dst, bool selected) {
    ThemeColors *col = theme_get_colors();
    int cx = dst->x + dst->w / 2;
    int cy = dst->y + dst->h / 2 + (int)ch->anim.offset_y;
    int hw = (int)(dst->w * ch->anim.scale / 2.0f);
    int hh = (int)(dst->h * ch->anim.scale / 2.0f);
    SDL_Rect scaled = { cx - hw, cy - hh, hw * 2, hh * 2 };
    SDL_Color bg = selected ? col->channel_selected : col->channel_bg;
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(renderer, &scaled);
    SDL_Color bd = col->channel_border;
    SDL_SetRenderDrawColor(renderer, bd.r, bd.g, bd.b, bd.a);
    SDL_RenderDrawRect(renderer, &scaled);
    if (ch->icon_tex) {
        SDL_Rect icon_rect = {
            scaled.x + 8,
            scaled.y + 8,
            scaled.w - 16,
            scaled.h - 28
        };
        SDL_RenderCopy(renderer, ch->icon_tex, NULL, &icon_rect);
    }
}

void channels_init(SDL_Renderer *renderer) {
    channel_grid_init(&g_grid);
    channel_grid_load(&g_grid, renderer, "config/channels.conf");
}

void channels_shutdown(void) {
    channel_grid_free(&g_grid);
}

void channels_update(void *ev) {
    (void)ev;
}

void channels_render(SDL_Renderer *renderer) {
    (void)renderer;
}
