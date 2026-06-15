#include "theme.h"
#include <stdio.h>
#include <stdlib.h>

static Theme current_theme = {
    .background_color = 0x1a1a1a,
    .text_color = 0xffffff,
    .accent_color = 0xff6b6b,
};

int theme_init() {
    printf("Initializing theme...\n");
    // Load theme from JSON or config
    return 1;
}

void theme_cleanup() {
    printf("Cleaning up theme...\n");
}

Theme* theme_get_current() {
    return &current_theme;
}
