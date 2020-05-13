#include "hitbox.h"
#include "player.h"
#include "base.h"
#include "rendering.h"

#include <iostream>

int sign(int x)
{
    return (x>0?1:(x?-1:0));
}

Hitbox::Hitbox(int x, int y, int sx, int sy): Object(x,y,"")
{
	fading = 0;

    hitbox_size[0] = size[0] = sx;
    hitbox_size[1] = size[1] = sy;

    pos[0] += size[0]/2;
    pos[1] += size[1]/2;

    animated_already = animating_now = false;

    gen_corners();
}

Hitbox::Hitbox(int x, int y, std::string s, bool load_as_animation, bool adjust_pos): Object(x,y,s,load_as_animation)
{
	fading = 0;

    if (adjust_pos)
    {
        pos[0] += size[0]/2;
        pos[1] += size[1]/2;
    }

    gen_corners();
}

void Hitbox::update(bool increase_anim_time)
{
    if (hitbox_size[0] > 0 && !fading && player->is_in_control() && collides(player))
    {
        if (drunkenness::auto_running)
        {
            player->hits++;
            if (player->hits == 1) player->change_movement(collide);
            else if (player->hits == 2) player->change_movement(fall);
            else if (player->hits == 3) player->change_movement(die);
        }
        else
        {
            if (player->pos[0] == pos[0] && player->pos[1] == pos[1]) ++player->pos[0];

            int dir = abs(player->pos[0]+player->hitbox_offset[0]-pos[0]-hitbox_offset[0]) <
                      abs(player->pos[1]+player->hitbox_offset[1]-pos[1]-hitbox_offset[1]);

            do
            {
                player->pos[dir] += sign(player->pos[dir]+player->hitbox_offset[dir]-pos[dir]-hitbox_offset[dir]);
                player->gen_corners();
            } while (collides(player));

            player->change_movement(collide);
        }

        fading = 1;

        //std::cout << hitbox_size[0] << " " << corners[0][0] << " " << corners[3][0] << " " << player->pos[0]-player->hitbox_size[0]/2 << "\n";
    }

    if (increase_anim_time && !animated_already && !fading)
    {
        if (animating_now)
        {
            Object::update(true);
            if (!cur_anim_frame && !cur_anim_time)
            {
                animated_already = true;
                animating_now = false;
            }
        }
        else if (!random(0,120) && pos[0] > camera[0]+30 && pos[0] < camera[0]+window[0]-30)
        {
            animating_now = true;
        }
    }

    if (fading)
	{
		fading += 10;
		--pos[1];
		if (fading > drunkenness::blur)
		{
			to_delete.push_back(this);
		}
	}
}

void Hitbox::render()
{
    SDL_Rect dest={pos[0]-size[0]/2-camera[0]+camera_x_offset, pos[1]-size[1]/2-camera[1], size[0], size[1]},
                src = {0, size[1]*cur_anim_frame, size[0], size[1]};

    SDL_SetTextureAlphaMod(anim->first,drunkenness::blur - fading);

    SDL_RenderCopyEx(renderer, anim->first, &src, &dest, rotation, nullptr, flipped?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
}

Pedestrian::Pedestrian(int x, int y, int direction, std::string s, bool load_as_animation) : Hitbox(x,y,s,load_as_animation,false)
{
    delete_on_restart.push_back(this);
    di = direction;
    flipped = di<0;

    if (!drunkenness::auto_running)
    {
        hitbox_size[0] = hitbox_size[1] = 0;
    }
    else
    {
        hitbox_size[0] = 9;
        hitbox_size[1] = 3;
        hitbox_offset[1] = 18;
    }
}

Pedestrian::~Pedestrian()
{
    remove_it(&delete_on_restart,(Object*)this);
}

void Pedestrian::update(bool increase_anim_time)
{
	if (!fading)
	{
		pos[0] += di;
		cur_anim_time += abs(di);
		gen_corners();
	}

    Hitbox::update(false);
    Object::update(false);
}
