#include "sound.h"
#include "base.h"

#include <map>

int sfx_volume, music_volume;
// TODO (Junber#1#): Add sounds and more music

void load_option_sound(std::string s, std::string s2)
{
    if (s == "sfx") sfx_volume = atoi(s2.c_str());
    else if (s == "music") music_volume = atoi(s2.c_str());
}

std::map<std::string,Mix_Chunk*> loaded_sounds;
Mix_Chunk* load_sound(std::string s)
{
    if (!loaded_sounds.count(s))
    {
        loaded_sounds[s] = Mix_LoadWAV((std::string("Data")+PATH_SEPARATOR+"Sounds"+PATH_SEPARATOR+s+".wav").c_str());
        Mix_VolumeChunk(loaded_sounds[s], sfx_volume);
    }
    return loaded_sounds[s];
}

std::map<std::string,Mix_Music*> loaded_music;
Mix_Music* load_music(std::string s)
{
    if (!loaded_music.count(s)) loaded_music[s] = Mix_LoadMUS((std::string("Data")+PATH_SEPARATOR+"Music"+PATH_SEPARATOR+s+".mp3").c_str());
    return loaded_music[s];
}

void sound_init()
{
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_VolumeMusic(music_volume);

    play_music(load_music("Barjazz"));
}

int selfcut_channel[3]={-1,-1};
void play_sound(Mix_Chunk* s, int selfcut)
{
    selfcut--;

    if (s!=nullptr)
    {
        int channel = Mix_PlayChannel((selfcut&&(selfcut_channel[selfcut]!=-1))?selfcut_channel[selfcut]:-1, s, 0);

        for (int i=0;i<=2;i++)
        {
            if (i == selfcut) selfcut_channel[i] = channel;
            else if (channel == selfcut_channel[selfcut]) selfcut_channel[selfcut] = -1;
        }
    }
}

void play_music(Mix_Music* s)
{
    Mix_PlayMusic(s,-1);
}

