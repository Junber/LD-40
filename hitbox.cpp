#include "hitbox.h"
#include "player.h"

#include <iostream>

int sign(int x)
{
    return (x>0?1:(x?-1:0));
}

Hitbox::Hitbox(int x, int y, int sx, int sy): Object(x,y,"")
{
    hitbox_size[0] = size[0] = sx;
    hitbox_size[1] = size[1] = sy;

    pos[0] += size[0]/2;
    pos[1] += size[1]/2;

    gen_corners();
}

Hitbox::Hitbox(int x, int y, std::string s, bool load_as_animation): Object(x,y,s,load_as_animation)
{
    pos[0] += size[0]/2;
    pos[1] += size[1]/2;

    gen_corners();
}

void Hitbox::update(bool increase_anim_time)
{
    if (collides(player))
    {
        if (player->pos[0] == pos[0] && player->pos[1] == pos[1]) ++player->pos[0];

        int dir = abs(player->pos[0]+player->hitbox_offset[0]-pos[0]-hitbox_offset[0]) <
                  abs(player->pos[1]+player->hitbox_offset[1]-pos[1]-hitbox_offset[1]);

        do
        {
            player->pos[dir] += sign(player->pos[dir]+player->hitbox_offset[dir]-pos[dir]-hitbox_offset[dir]);
            player->gen_corners();
        } while (collides(player));

        //std::cout << hitbox_size[0] << " " << corners[0][0] << " " << corners[3][0] << " " << player->pos[0]-player->hitbox_size[0]/2 << "\n";
    }
}
