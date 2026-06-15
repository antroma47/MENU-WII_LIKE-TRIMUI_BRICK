#ifndef WIDGETS_H
#define WIDGETS_H

#include <SDL2/SDL.h>

void widgets_init(SDL_Renderer *renderer);
void widgets_update(void);
void widgets_render(SDL_Renderer *renderer);
void widgets_shutdown(void);

#endif
