#ifndef _HITBOX
#define _HITBOX

#include "object.h"

class Hitbox: public Object
{
public:
    Hitbox(int x, int y, int sx, int sy);
    Hitbox(int x, int y, std::string s, bool load_as_animation = false);

    void update(bool increase_anim_time=true);
};

#endif // _HITBOX
