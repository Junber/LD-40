#ifndef _BASE
#define _BASE

#include <deque>
#include <string>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

namespace drunkenness
{
    extern int blick_frequency, blur;
}

int random(int x, int y);
void random_init();
std::deque<std::string> split(std::string s, char seperator);

#endif // _BASE
