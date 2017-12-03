#ifndef _ENTRANCE
#define _ENTRANCE

#include "object.h"

class Entrance: public Object
{
public:
    Entrance(int x, int y, int sx, int sy);

    void enter();
    void update(bool increase_anim_time=true);
    void render() override {}
};

#endif // _ENTRANCE
