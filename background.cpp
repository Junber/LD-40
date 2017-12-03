#include "background.h"
#include "rendering.h"
#include "player.h"
#include "base.h"

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

int walls_end_at=-window[0], sidewalks_end_at=-window[0], streets_end_at=-window[0];

bool add_new_backgrounds()
{
    bool did_something=false;

    if (player->pos[0]+window[0] >= walls_end_at)
    {
        auto b = new Background(walls_end_at,0,"wall",true);
        if (!random(0,20)) b->cur_anim_frame=1;
        else if (!random(0,3)) new Background(walls_end_at+random(10,b->size[0]-50),random(10,b->size[1]-60),"poster",true);

        walls_end_at += b->size[0];

        did_something=true;
    }
    if (player->pos[0]+window[0] >= sidewalks_end_at)
    {
        auto b = new Background(sidewalks_end_at,90,"sidewalk",true);
        if (!random(0,5)) b->cur_anim_frame=1;

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
