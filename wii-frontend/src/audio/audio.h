#ifndef AUDIO_H
#define AUDIO_H

int audio_init();
void audio_cleanup();
void audio_play(const char *sound_name);

#endif
