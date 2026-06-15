#include "input.h"
#include <stdio.h>
#include <string.h>

void input_init(void) {
    printf("[input] initialized\n");
}

void input_poll(InputEvent *ev) {
    if (!ev) return;
    memset(ev, 0, sizeof(InputEvent));
    SDL_Event sdl_event;
    if (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
        case SDL_QUIT:
            ev->type = INPUT_QUIT;
            break;
        case SDL_KEYDOWN:
            switch (sdl_event.key.keysym.sym) {
            case SDLK_UP:
                ev->type = INPUT_UP;
                break;
            case SDLK_DOWN:
                ev->type = INPUT_DOWN;
                break;
            case SDLK_LEFT:
                ev->type = INPUT_LEFT;
                break;
            case SDLK_RIGHT:
                ev->type = INPUT_RIGHT;
                break;
            case SDLK_RETURN:
                ev->type = INPUT_CONFIRM;
                break;
            case SDLK_ESCAPE:
                ev->type = INPUT_CANCEL;
                break;
            default:
                ev->type = INPUT_NONE;
            }
            ev->key = sdl_event.key.keysym.sym;
            break;
        default:
            ev->type = INPUT_NONE;
        }
    }
}

void input_shutdown(void) {
    printf("[input] shutdown\n");
}
