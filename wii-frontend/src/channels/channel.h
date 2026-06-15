#ifndef CHANNEL_H
#define CHANNEL_H

typedef struct {
    char name[64];
    char path[256];
    char icon_path[256];
} Channel;

void channel_init();
void channel_cleanup();
Channel* channel_get_list(int *count);

#endif
