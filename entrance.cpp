#include "entrance.h"
#include "player.h"
#include "base.h"
#include "rendering.h"
#include "dialog.h"

#include <iostream>

Entrance::Entrance(int x, int y, int sx, int sy, Background* door_to_be_opened): Object(x,y,"empty")
{
    hitbox_size[0] = size[0] = sx;
    hitbox_size[1] = size[1] = sy;
    door = door_to_be_opened;
    gen_corners();

    opening = false;
}

void Entrance::enter()
{
    player->save_pos();
    dialog("test", load_image("bar"));
    delete this;
}

void Entrance::update(bool increase_anim_time)
{
    if (opening)
    {
        //std::cout << door->cur_anim_time << "\n";
        door->Object::update();
        if (!door->cur_anim_frame && !door->cur_anim_time)
        {
            player->change_movement(drunkenness::auto_running?auto_runner:in_control);
            enter();
        }
    }
    else
    {
        if (collides(player))
        {
            player->change_movement(stop);
            opening = true;
        }
        else if (drunkenness::auto_running && player->pos[0]+player->size[0]/2 >= pos[0]+size[0]/2) player->change_movement(walking_up);
    }
}
