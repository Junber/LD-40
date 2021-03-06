#ifndef _HITBOX
#define _HITBOX

#include "object.h"

class Hitbox: public Object
{
public:
	int fading;
    bool animated_already, animating_now;
    Hitbox(int x, int y, int sx, int sy);
    Hitbox(int x, int y, std::string s, bool load_as_animation = false, bool adjust_pos=true);

    void update(bool increase_anim_time=true);
    void render();
};

class Pedestrian: public Hitbox
{
public:
    int di;
    Pedestrian(int x, int y, int direction, std::string s, bool load_as_animation=false);
    ~Pedestrian();

    void update(bool increase_anim_time=true);
};
#endif // _HITBOX
