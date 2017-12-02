#ifndef _PLAYER
#define _PLAYER

#include "object.h"

class Player: public Object
{
    const int movement_speed = 2;

    void update_camera();

public:
    Player();
    void update(bool increase_anim_time=true);
};

#endif // _PLAYER
