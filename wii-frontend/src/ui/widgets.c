#include "widgets.h"
#include <stdio.h>

void widgets_init(SDL_Renderer *renderer) {
    (void)renderer;
    printf("[widgets] initialized\n");
}

void widgets_update(void) {
}

void widgets_render(SDL_Renderer *renderer) {
    (void)renderer;
}

void widgets_shutdown(void) {
    printf("[widgets] shutdown\n");
}
