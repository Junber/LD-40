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

class Player: public Object
{
    const int movement_speed = 2;

public:
    Player(): Object(0,0,"walk1")
    {
        anim.second[0] = 5;
        for (int i=0; i<5; ++i) anim.second.push_back(5);
        size[1] /= 6;

        hitbox_size[1] = 6;
        hitbox_offset[1] = 18;

        gen_corners();
    }

    void update(bool increase_anim_time=true)
    {
        const Uint8* state = SDL_GetKeyboardState(nullptr);

        int last_pos[2] = {pos[0], pos[1]};

        if (state[SDL_SCANCODE_D]) pos[0] += movement_speed;
        else if (state[SDL_SCANCODE_A]) pos[0] -= movement_speed;
        if (state[SDL_SCANCODE_S]) pos[1] += movement_speed;
        else if (state[SDL_SCANCODE_W]) pos[1] -= movement_speed;

        if (last_pos[0] != pos[0]  || last_pos[1] != pos[1])
        {
            //rotation = std::atan2(pos[1]-last_pos[1], pos[0]-last_pos[0])*180/M_PI;
            gen_corners();
            cur_anim_time++;
        }

        if (last_pos[0] != pos[0]) flipped = last_pos[0] > pos[0];

        Object::update(false);
    }
};

Player* player;

int sign(int x)
{
    return (x>0?1:(x?-1:0));
}

class Hitbox: public Object
{
public:
    Hitbox(int x, int y, int sx, int sy): Object(x,y,"")
    {
        size[0] = sx;
        size[1] = sy;
        gen_corners();
    }

    Hitbox(int x, int y, std::string s): Object(x,y,s){}

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

void visual_novel(SDL_Texture* tex, std::string text)
{
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    if (e.key.keysym.sym == SDLK_ESCAPE) breakk = true;
			}
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer,tex,nullptr,nullptr);

        render_text(10,100,add_newlines(text,300),100);

        SDL_RenderPresent(renderer);
        limit_fps();
    }
}

bool sort_criteria(Object* a, Object* b)
{
    return a->pos[1]+a->hitbox_offset[1] < b->pos[1]+b->hitbox_offset[1];
}

int main(int argc, char* args[])
{
    int blick_progress = -1;

    random_init();
    render_init();
    font_init();

    player = new Player();

    new Hitbox(100,100,"Player");

    //SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    if (e.key.keysym.sym == SDLK_ESCAPE) breakk = true;
			    if (e.key.keysym.sym == SDLK_e) visual_novel(load_image("Player"),"Hello. This text is a test. Testing. Testing! TESING!! Hopefully this works.");
			}
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        player->update();
        for (Object* o: objects)
        {
            if (o != player) o->update();
        }

        std::stable_sort(objects.begin(), objects.end(), sort_criteria);
        for (Object* o: objects)
        {
            o->render();
        }

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

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
