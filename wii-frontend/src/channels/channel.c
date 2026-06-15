#include "channel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Channel *channels = NULL;
static int channel_count = 0;

void channel_init() {
    printf("Initializing channels...\n");
    // Load channels from configuration
}

void channel_cleanup() {
    if (channels) {
        free(channels);
        channels = NULL;
    }
}

Channel* channel_get_list(int *count) {
    *count = channel_count;
    return channels;
}
