#include "background.h"
#include "rendering.h"
#include "player.h"
#include "base.h"
#include "hitbox.h"
#include "entrance.h"

#include <iostream>

std::deque<Background*> backgrounds;

Background::Background(int x, int y, std::string s, bool load_as_animation): Object(x,y,s,load_as_animation)
{
    pos[0] += size[0]/2;
    pos[1] += size[1]/2;

    //std::cout << pos[0] << "\n";

    remove_it(&objects, (Object*) this);
    backgrounds.push_back(this);
}

Background::~Background()
{
    remove_it(&backgrounds, this);
}

int walls_end_at=-window[0], sidewalks_end_at=-window[0], streets_end_at=-window[0], sidewalks_since_lantern=0, walls_since_bar=0;

bool add_new_backgrounds()
{
    bool did_something=false;

    if (player->pos[0]+window[0] >= walls_end_at)
    {
        auto b = new Background(walls_end_at,0,"wall",true);

        walls_since_bar++;
        if (walls_since_bar > 40-player->drunk_level*5)
        {
            b->cur_anim_frame=2;
            new Entrance(walls_end_at+65,0,30,b->size[1]+80);
            walls_since_bar = 0;
        }
        else if (!random(0,20)) b->cur_anim_frame=1;
        else if (!random(0,3)) new Background(walls_end_at+random(10,b->size[0]-50),random(10,b->size[1]-60),"poster",true);

        walls_end_at += b->size[0];

        did_something=true;
    }
    if (player->pos[0]+window[0] >= sidewalks_end_at)
    {
        auto b = new Background(sidewalks_end_at,90,"sidewalk",true);
        if (!random(0,5)) b->cur_anim_frame=1;

        if (!random(0,5))
        {
            Hitbox* h = new Hitbox(sidewalks_end_at,random(91,114),"bin",true);
            if (!random(0,1)) b->cur_anim_frame=1;

            h->hitbox_size[1] = 8;
            h->hitbox_offset[1] = 9;
            h->gen_corners();

            sidewalks_since_lantern = 0;
        }

        sidewalks_since_lantern++;
        if (sidewalks_since_lantern >= 5)
        {
            Hitbox* h = new Hitbox(sidewalks_end_at,0,"lantern",true);

            h->hitbox_size[0] = 17;
            h->hitbox_size[1] = 8;
            h->hitbox_offset[0] = 30;
            h->hitbox_offset[1] = 60;
            h->gen_corners();

            sidewalks_since_lantern = 0;
        }

        sidewalks_end_at += b->size[0];

        did_something=true;
    }
    if (player->pos[0]+window[0] >= streets_end_at)
    {
        auto b = new Background(streets_end_at,140,"street",true);
        if (!random(0,3)) b->cur_anim_frame=1;

        streets_end_at += b->size[0];

        did_something=true;
    }

    return did_something;
}
