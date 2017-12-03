#ifndef _PLAYER
#define _PLAYER

#include "object.h"

enum player_movement
{
    in_control, stumble
};

class Player;
extern Player* player;
class Player: public Object
{
    const int movement_speed = 2;
    player_movement cur_movement;

    void update_camera();

public:
    int drunk_level;

    Player();
    void update(bool increase_anim_time=true);
    void change_movement(player_movement m);
};

#endif // _PLAYER
