#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>

#include "object.h"
#include "rendering.h"
#include "font.h"
#include "base.h"
#include "player.h"
#include "background.h"

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

bool breakk = false;

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}

int sign(int x)
{
    return (x>0?1:(x?-1:0));
}

class Hitbox: public Object
{
public:
    Hitbox(int x, int y, int sx, int sy): Object(x,y,"")
    {
        hitbox_size[0] = size[0] = sx;
        hitbox_size[1] = size[1] = sy;
        gen_corners();
    }

    Hitbox(int x, int y, std::string s, bool load_as_animation = false): Object(x,y,s,load_as_animation){}

    void update(bool increase_anim_time=true)
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
        }
    }
};

class Entrance;
std::deque<Entrance*> entraces;
class Entrance: public Object
{
public:
    Entrance(int x, int y, int sx, int sy): Object(x,y,"")
    {
        hitbox_size[0] = size[0] = sx;
        hitbox_size[1] = size[1] = sy;
        gen_corners();

        remove_it(&objects, (Object*) this);
        entraces.push_back(this);
    }

    ~Entrance() override
    {
        remove_it(&entraces, this);
    }

    void enter()
    {
        //visual_novel()
    }

    void update(bool increase_anim_time=true) override {}
    void render() override {}
};

bool visual_novel(SDL_Texture* tex, std::string text, std::string text2="")
{
    bool choice = !text2.empty(), decision;

    int move_out = 0;

    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    if (e.key.keysym.sym == SDLK_ESCAPE) breakk = true;
			    else if (e.key.keysym.sym == SDLK_e && !choice) return 0;
			    else if (e.key.keysym.sym == SDLK_a && choice)
                {
                    decision = 0;
                    move_out = 1;
                }
			    else if (e.key.keysym.sym == SDLK_d && choice)
                {
                    decision = 1;
                    move_out = 1;
                }
			}
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,tex,nullptr,nullptr);

        if (choice)
        {
            if (move_out>0) move_out++;

            render_text(10 +(decision?0:move_out),100+(decision?move_out:0),add_newlines(text ,145),100,std::max(0,255-7*(decision?move_out:0)));
            render_text(155-(decision?move_out:0),100+(decision?0:move_out),add_newlines(text2,145),100,std::max(0,255-7*(decision?0:move_out)));

            if (move_out >= 100) return decision;
        }
        else render_text(10,100,add_newlines(text,300),100);

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}

bool sort_criteria(Object* a, Object* b)
{
    return a->pos[1]+a->hitbox_offset[1] < b->pos[1]+b->hitbox_offset[1];
}

int blick_progress = -1;

void blinking()
{
    if (blick_progress >= 0)
    {
        ++blick_progress;

        SDL_SetRenderDrawColor(renderer,0,0,0,255);

        if (blick_progress < 10)
        {
            SDL_Rect r = {0,0,window[0],blick_progress*window[1]/10};
            SDL_RenderFillRect(renderer,&r);

            r.y = window[1]-r.h;
            SDL_RenderFillRect(renderer,&r);
        }
        else if (blick_progress < 40)
        {
            SDL_RenderClear(renderer);
        }
        else if (blick_progress < 50)
        {
            SDL_Rect r = {0,0,window[0],(50-blick_progress)*window[1]/10};
            SDL_RenderFillRect(renderer,&r);

            r.y = window[1]-r.h;
            SDL_RenderFillRect(renderer,&r);
        }
        else blick_progress = -1;
    }
    else if (!random(0,drunkenness::blick_frequency))
    {
        ++blick_progress;
    }
}

void update()
{
    player->update();
    for (Object* o: objects)
    {
        if (o != player) o->update();
    }

    blinking();
}

void render()
{
    for (Background* o: backgrounds)
    {
        o->render();
    }
    std::stable_sort(objects.begin(), objects.end(), sort_criteria);
    for (Object* o: objects)
    {
        SDL_SetTextureAlphaMod(o->anim->first,drunkenness::blur);
        o->render();
    }

    blinking();
}

int main(int argc, char* args[])
{
    random_init();
    render_init();
    font_init();

    player = new Player();

    while (add_new_backgrounds()) {}

    SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    if (e.key.keysym.sym == SDLK_ESCAPE) breakk = true;
			    else if (e.key.keysym.sym == SDLK_e)
                {
                    for (Entrance* ent: entraces)
                    {
                        if (ent->collides(player)) ent->enter();
                    }
                }
			    else if (e.key.keysym.sym == SDLK_r) visual_novel(load_image("Player"),"Hello. This text is a test. Testing. Testing! TESING!! Hopefully this works.");
			    else if (e.key.keysym.sym == SDLK_q) visual_novel(load_image("Player"),"Choice 1","And the far superior choice 2");
			}
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,drunkenness::blur);
        SDL_RenderFillRect(renderer,nullptr);

        update();
        add_new_backgrounds();
        render();

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
