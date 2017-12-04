#include "entrance.h"
#include "player.h"
#include "base.h"
#include "rendering.h"
#include "dialog.h"
#include "sound.h"

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
    int patron_num = 5-player->drunk_level+(player->alcohol_points < 7), img_num;
    switch (patron_num)
    {
    case 5:
    case 0:
        img_num = 0;
        break;
    case 4:
        img_num = random(0,4);
        break;
    case 3:
        img_num = random(0,7);
        break;
    case 2:
        img_num = random(0,9);
        break;
    case 1:
        img_num = random(0,4);
    }

    player->save_pos();
    dialog(std::to_string(player->drunk_level)+std::to_string(player->dialog_this_level+1), load_image("bar_"+std::to_string(patron_num)+std::to_string(img_num)));
    player->dialog_this_level++;
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
            play_sound(load_sound("door_open"));
        }
        else if (drunkenness::auto_running && player->pos[0]+player->size[0]/2 >= pos[0]+size[0]/2) player->change_movement(walking_up);
    }
}
