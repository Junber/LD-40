#ifndef _ENTRANCE
#define _ENTRANCE

#include "object.h"
#include "background.h"

class Entrance: public Object
{
public:
    bool opening;
    Background* door;
    Entrance(int x, int y, int sx, int sy, Background* door_to_be_opened);

    void enter();
    void update(bool increase_anim_time=true);
    void render() override {}
};

#endif // _ENTRANCE
