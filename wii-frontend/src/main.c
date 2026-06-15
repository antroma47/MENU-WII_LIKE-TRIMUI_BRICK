#include <stdio.h>
#include <stdlib.h>
#include "ui/ui.h"
#include "audio/audio.h"
#include "theme/theme.h"

int main(int argc, char *argv[]) {
    printf("WiiMenu for TrimUI Brick - Starting...\n");
    
    // Initialize theme
    if (!theme_init()) {
        fprintf(stderr, "Failed to initialize theme\n");
        return 1;
    }
    
    // Initialize audio
    if (!audio_init()) {
        fprintf(stderr, "Failed to initialize audio\n");
        return 1;
    }
    
    // Initialize UI
    if (!ui_init()) {
        fprintf(stderr, "Failed to initialize UI\n");
        return 1;
    }
    
    // Main loop
    ui_run();
    
    // Cleanup
    ui_cleanup();
    audio_cleanup();
    theme_cleanup();
    
    printf("WiiMenu closed.\n");
    return 0;
}
