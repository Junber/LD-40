#ifndef _BASE
#define _BASE

#include <deque>
#include <string>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

void limit_fps();

namespace drunkenness
{
    extern int blick_frequency, blur, swaying;
    extern bool speed_randomness,auto_running, random_keys;
    extern int base_movement_speed, movement_speed; //for negative numbers: -x = 1/x
}

extern bool breakk;

int random(int x, int y);
void random_init();
std::deque<std::string> split(std::string s, char seperator);

#endif // _BASE
