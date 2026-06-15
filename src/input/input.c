#include "input.h"
#include <SDL2/SDL.h>
#include <string.h>

/* Stato tasti per held detection */
static bool key_state[KEY_COUNT] = {0};

/* Mappatura SDL scancode → KeyCode TrimUI */
static KeyCode sdl_to_key(SDL_Keycode sym) {
    switch (sym) {
        case SDLK_UP:       return KEY_UP;
        case SDLK_DOWN:     return KEY_DOWN;
        case SDLK_LEFT:     return KEY_LEFT;
        case SDLK_RIGHT:    return KEY_RIGHT;
        case SDLK_RETURN:   return KEY_A;
        case SDLK_ESCAPE:   return KEY_B;
        case SDLK_x:        return KEY_X;
        case SDLK_y:        return KEY_Y;
        case SDLK_q:        return KEY_L;
        case SDLK_e:        return KEY_R;
        case SDLK_RETURN2:  return KEY_START;
        case SDLK_RSHIFT:   return KEY_SELECT;
        case SDLK_HOME:     return KEY_MENU;
        default:            return KEY_NONE;
    }
}

/* Mappatura gamepad button → KeyCode */
static KeyCode pad_to_key(Uint8 button) {
    switch (button) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:    return KEY_UP;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:  return KEY_DOWN;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:  return KEY_LEFT;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return KEY_RIGHT;
        case SDL_CONTROLLER_BUTTON_A:          return KEY_A;
        case SDL_CONTROLLER_BUTTON_B:          return KEY_B;
        case SDL_CONTROLLER_BUTTON_X:          return KEY_X;
        case SDL_CONTROLLER_BUTTON_Y:          return KEY_Y;
        case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  return KEY_L;
        case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return KEY_R;
        case SDL_CONTROLLER_BUTTON_START:      return KEY_START;
        case SDL_CONTROLLER_BUTTON_BACK:       return KEY_SELECT;
        case SDL_CONTROLLER_BUTTON_GUIDE:      return KEY_MENU;
        default:                               return KEY_NONE;
    }
}

static SDL_GameController *controller = NULL;

void input_init(void) {
    memset(key_state, 0, sizeof(key_state));

    /* Apri primo controller disponibile */
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) break;
        }
    }
}

void input_poll(InputEvent *ev) {
    ev->type = INPUT_NONE;
    ev->key  = KEY_NONE;

    SDL_Event sdl_ev;
    while (SDL_PollEvent(&sdl_ev)) {
        switch (sdl_ev.type) {

            case SDL_QUIT:
                ev->type = INPUT_QUIT;
                return;

            case SDL_KEYDOWN: {
                KeyCode k = sdl_to_key(sdl_ev.key.keysym.sym);
                if (k != KEY_NONE) {
                    key_state[k] = true;
                    ev->type = INPUT_KEYDOWN;
                    ev->key  = k;
                    return;
                }
                break;
            }

            case SDL_KEYUP: {
                KeyCode k = sdl_to_key(sdl_ev.key.keysym.sym);
                if (k != KEY_NONE) {
                    key_state[k] = false;
                    ev->type = INPUT_KEYUP;
                    ev->key  = k;
                    return;
                }
                break;
            }

            case SDL_CONTROLLERBUTTONDOWN: {
                KeyCode k = pad_to_key(sdl_ev.cbutton.button);
                if (k != KEY_NONE) {
                    key_state[k] = true;
                    ev->type = INPUT_KEYDOWN;
                    ev->key  = k;
                    return;
                }
                break;
            }

            case SDL_CONTROLLERBUTTONUP: {
                KeyCode k = pad_to_key(sdl_ev.cbutton.button);
                if (k != KEY_NONE) {
                    key_state[k] = false;
                    ev->type = INPUT_KEYUP;
                    ev->key  = k;
                    return;
                }
                break;
            }

            case SDL_CONTROLLERDEVICEADDED:
                if (!controller) {
                    controller = SDL_GameControllerOpen(sdl_ev.cdevice.which);
                }
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                if (controller) {
                    SDL_GameControllerClose(controller);
                    controller = NULL;
                }
                break;
        }
    }
}

bool input_is_held(KeyCode key) {
    if (key <= KEY_NONE || key >= KEY_COUNT) return false;
    return key_state[key];
}

void input_shutdown(void) {
    if (controller) {
        SDL_GameControllerClose(controller);
        controller = NULL;
    }
}
