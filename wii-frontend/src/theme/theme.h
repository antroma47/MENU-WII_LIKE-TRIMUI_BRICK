#ifndef THEME_H
#define THEME_H

typedef struct {
    unsigned int background_color;
    unsigned int text_color;
    unsigned int accent_color;
    char font_path[256];
} Theme;

int theme_init();
void theme_cleanup();
Theme* theme_get_current();

#endif
