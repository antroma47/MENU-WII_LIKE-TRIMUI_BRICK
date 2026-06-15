#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ui/ui.h"
#include "input/input.h"
#include "audio/audio.h"
#include "theme/theme.h"

/* TrimUI Brick screen resolution */
#define SCREEN_W 1024
#define SCREEN_H 768
#define FPS_TARGET 60
#define FRAME_DELAY (1000 / FPS_TARGET)

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    UI            ui;
    bool          running;
    Uint32        last_frame;
} App;

static bool app_init(App *app) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return false;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init error: %s\n", TTF_GetError());
        return false;
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
        fprintf(stderr, "IMG_Init error: %s\n", IMG_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0) {
        fprintf(stderr, "Mix_OpenAudio error: %s\n", Mix_GetError());
        /* audio non bloccante, continuiamo */
    }

    app->window = SDL_CreateWindow(
        "Wii Menu TrimUI",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_W, SCREEN_H,
        SDL_WINDOW_FULLSCREEN
    );

    if (!app->window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        return false;
    }

    app->renderer = SDL_CreateRenderer(
        app->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!app->renderer) {
        /* fallback software renderer */
        app->renderer = SDL_CreateRenderer(
            app->window, -1,
            SDL_RENDERER_SOFTWARE
        );
        if (!app->renderer) {
            fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
            return false;
        }
    }

    SDL_RenderSetLogicalSize(app->renderer, SCREEN_W, SCREEN_H);

    app->running    = true;
    app->last_frame = SDL_GetTicks();

    return true;
}

static void app_shutdown(App *app) {
    audio_shutdown();
    theme_shutdown();
    input_shutdown();

    if (app->renderer) SDL_DestroyRenderer(app->renderer);
    if (app->window)   SDL_DestroyWindow(app->window);

    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;

    App app = {0};

    if (!app_init(&app)) {
        return EXIT_FAILURE;
    }

    /* Inizializza sottosistemi */
    theme_init(app.renderer);
    audio_init();
    input_init();
    ui_init(&app.ui, app.renderer, SCREEN_W, SCREEN_H);

    InputEvent ev = {0};
    float dt = 1.0f / FPS_TARGET;  /* deltatime in secondi */

    while (app.running) {

        /* Input */
        input_poll(&ev);

        if (ev.type == INPUT_QUIT) {
            app.running = false;
        }

        /* Update UI */
        ui_update(&app.ui, dt);

        /* Render */
        SDL_SetRenderDrawColor(app.renderer, 10, 20, 40, 255);
        SDL_RenderClear(app.renderer);

        theme_render_background(app.renderer);
        ui_render(&app.ui);

        SDL_RenderPresent(app.renderer);

        /* Cap FPS */
        Uint32 now   = SDL_GetTicks();
        Uint32 delta = now - app.last_frame;
        if (delta < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - delta);
        }
        app.last_frame = SDL_GetTicks();
    }

    app_shutdown(&app);
    return EXIT_SUCCESS;
}
