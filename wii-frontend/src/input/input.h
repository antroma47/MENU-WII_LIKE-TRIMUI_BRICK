#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

typedef enum {
    INPUT_NONE = 0,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_CONFIRM,
    INPUT_CANCEL,
    INPUT_QUIT
} InputType;

typedef struct {
    InputType type;
    SDL_Keycode key;
} InputEvent;

void input_init(void);
void input_poll(InputEvent *ev);
void input_shutdown(void);

#endif
