#ifndef _SOUND
#define _SOUND

#include <string>
#include <SDL_mixer.h>

void load_option_sound(std::string s, std::string s2);

Mix_Chunk* load_sound(std::string s);
Mix_Music* load_music(std::string s);

void sound_init();
void play_sound(Mix_Chunk* s, int selfcut=0);
void play_music(Mix_Music* s);

#endif // _SOUND
