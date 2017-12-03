#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "object.h"
#include "rendering.h"
#include "font.h"
#include "base.h"
#include "player.h"
#include "background.h"
#include "sound.h"
#include "dialog.h"

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

bool sway_at_next_opportunity=false;
void update()
{
    if (drunkenness::speed_randomness && !random(0,30))
    {
        drunkenness::movement_speed = drunkenness::base_movement_speed+random(-1,1);
        std::cout << drunkenness::movement_speed;
    }

    if (sway_at_next_opportunity || (drunkenness::swaying && player->is_in_control() &&!random(0,drunkenness::swaying)))
    {
        if (player->cur_anim_frame==2)
        {
            player->change_movement(sway);
            sway_at_next_opportunity = false;
        }
        else sway_at_next_opportunity = true;
    }

    player->update();
    for (Object* o: objects)
    {
        if (o != player) o->update();
    }
}

void load_options()
{
    std::fstream file;
    file.open("options.txt");
    std::string line;
    while (!file.eof())
    {
        std::getline(file, line);
        if (!line.empty() && line[0] != '#')
        {
            auto sp = split(line,':');

            load_option_rendering(sp[0],sp[1]);
            load_option_sound(sp[0],sp[1]);
        }
    }
    file.close();
}

int main(int argc, char* args[])
{
    load_options();
    load_drinks();

    random_init();
    render_init();
    font_init();
    sound_init();

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
			    else if (e.key.keysym.sym == SDLK_r) player->change_movement(sway);
			    else if (e.key.keysym.sym == SDLK_q) dialog("test",load_image("bar"));
			    else if (e.key.keysym.sym == SDLK_f) player->change_movement(stumble);
			}
        }

        update();
        add_new_backgrounds();
        render();

        //std::cout << camera[0];

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
