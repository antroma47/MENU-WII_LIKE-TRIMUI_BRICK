#include "ui.h"
#include "banner.h"
#include <stdio.h>

int ui_init() {
    printf("Initializing UI...\n");
    banner_init();
    return 1;
}

void ui_run() {
    printf("Running UI main loop...\n");
    // Main event loop
}

void ui_cleanup() {
    printf("Cleaning up UI...\n");
    banner_cleanup();
}
