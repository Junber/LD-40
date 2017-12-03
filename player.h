#ifndef _PLAYER
#define _PLAYER

#include "object.h"

enum player_movement
{
    auto_runner, in_control, stumble, sway, walking_up
};

class Player;
extern Player* player;
class Player: public Object
{
    int time_till_move=0, saved_pos[2];
    player_movement cur_movement;

    void update_camera();

public:
    int drunk_level;

    Player();
    void update(bool increase_anim_time=true);
    bool is_in_control();
    void change_movement(player_movement m);
    void save_pos();
    void kill();
};

#endif // _PLAYER
