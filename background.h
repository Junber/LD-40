#ifndef _BACKGROUND
#define _BACKGROUND

#include "object.h"

class Background;
extern std::deque<Background*> backgrounds;

class Background: public Object
{
public:
    Background(int x, int y, std::string s, bool load_as_animation = false);

    ~Background() override;

    void update(bool increase_anim_time=true) override {}
};

bool add_new_backgrounds();

#endif // _BACKGROUND
