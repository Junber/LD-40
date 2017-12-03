#include "entrance.h"
#include "player.h"
#include "base.h"
#include "rendering.h"
#include "dialog.h"

Entrance::Entrance(int x, int y, int sx, int sy): Object(x,y,"empty")
{
    hitbox_size[0] = size[0] = sx;
    hitbox_size[1] = size[1] = sy;
    gen_corners();

}

void Entrance::enter()
{
    dialog("test", load_image("bar"));
    delete this;
}

void Entrance::update(bool increase_anim_time)
{
    if (collides(player))
    {
        player->change_movement(drunkenness::auto_running?auto_runner:in_control);
        enter();
    }
    else if (drunkenness::auto_running && player->pos[0]+player->size[0]/2 >= pos[0]+size[0]/2) player->change_movement(walking_up);
}
