#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>

#include "object.h"
#include "rendering.h"

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
    Player(): Object(0,0,"Player") {}

    void update()
    {
        const Uint8* state = SDL_GetKeyboardState(nullptr);

        if (state[SDL_SCANCODE_D]) pos[0] += movement_speed;
        else if (state[SDL_SCANCODE_A]) pos[0] -= movement_speed;
        if (state[SDL_SCANCODE_S]) pos[1] += movement_speed;
        else if (state[SDL_SCANCODE_W]) pos[1] -= movement_speed;

        gen_corners();
    }
};

class Hitbox: public Object
{
public:
    Hitbox(int x, int y, int sx, int sy): Object(x,y,"")
    {
        size[0] = sx;
        size[1] = sy;
        gen_corners();
    }

    void update()
    {

    }

    void render() {}
};

int main(int argc, char* args[])
{
    render_init();

    Player* player = new Player();

    Object* obj = new Object(20,20,"Player");

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
			    if (e.key.keysym.sym == SDLK_e) player->rotation+=45;
			}
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        for (Object* o: objects)
        {
            o->update();
            o->render();
        }

        std::cout << player->collides(obj);

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
