#ifndef _BASE
#define _BASE

#include <deque>
#include <string>

namespace drunkenness
{
    extern int blick_frequency, blur;
}

int random(int x, int y);
void random_init();
std::deque<std::string> split(std::string s, char seperator);

#endif // _BASE
