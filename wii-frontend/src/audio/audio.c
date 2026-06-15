#include "audio.h"
#include <stdio.h>

int audio_init() {
    printf("Initializing audio...\n");
    return 1;
}

void audio_cleanup() {
    printf("Cleaning up audio...\n");
}

void audio_play(const char *sound_name) {
    printf("Playing sound: %s\n", sound_name);
}
