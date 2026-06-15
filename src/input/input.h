#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

/* Tasti fisici TrimUI Brick */
typedef enum {
    KEY_NONE   = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_A,        /* conferma */
    KEY_B,        /* indietro */
    KEY_X,
    KEY_Y,
    KEY_L,        /* pagina sinistra */
    KEY_R,        /* pagina destra */
    KEY_START,
    KEY_SELECT,
    KEY_MENU,     /* tasto funzione home */
    KEY_COUNT
} KeyCode;

typedef enum {
    INPUT_NONE = 0,
    INPUT_KEYDOWN,
    INPUT_KEYUP,
    INPUT_QUIT
} InputEventType;

typedef struct {
    InputEventType type;
    KeyCode        key;
} InputEvent;

void input_init(void);
void input_poll(InputEvent *ev);
void input_shutdown(void);

/* Repeat automatico per navigazione tenendo premuto */
bool input_is_held(KeyCode key);

#endif /* INPUT_H */
