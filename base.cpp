#include "base.h"
#include <random>
#include <time.h>
#include <SDL.h>

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}

namespace drunkenness
{
    int blick_frequency = 1200000, blur = 255, cur_speed_difference=0, swaying=0;
    bool speed_randomness=false, auto_running=false, random_keys=false;
    int base_movement_speed=2, movement_speed = base_movement_speed; //for negative numbers: -x = 1/x
}

bool breakk = false;

std::default_random_engine generator;
int random(int x, int y)
{
    std::uniform_int_distribution<int> distribution(x,y);
    return distribution(generator);
}

void random_init()
{
    generator.seed(time(nullptr));
    random(0,1);
}

std::deque<std::string> split(std::string s, char seperator)
{
    std::deque<std::string> ret;
    ret.push_back("");
    for (char c: s)
    {
        if (c == seperator)
        {
            ret.push_back("");
        }
        else
        {
            ret[ret.size()-1] += c;
        }
    }

    return ret;
}
