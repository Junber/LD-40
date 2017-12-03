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

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

bool breakk = false;

struct drink
{
    std::string name;
    int alcohol;
};

std::deque<drink*> drinks;
void load_drinks()
{
    std::fstream file;
    file.open(std::string("Data")+PATH_SEPARATOR+"Dialog"+PATH_SEPARATOR+"DRINKS.txt");
    std::string line;

    while (!file.eof())
    {
        std::getline(file,line);
        auto sp = split(line, ':');

        drink* d = new drink();
        d->name = sp[0];
        d->alcohol = std::stoi(sp[1]);
        drinks.push_back(d);
    }
    file.close();
}

drink* random_drink()
{
    return drinks[random(0,drinks.size()-1)];
}

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}

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

            render_text(10 +(decision?0:move_out)/2,100+(decision?move_out:0),add_newlines(text ,145),100,std::max(0,255-7*(decision?move_out:0)));
            render_text(155-(decision?move_out:0)/2,100+(decision?0:move_out),add_newlines(text2,145),100,std::max(0,255-7*(decision?0:move_out)));

            if (move_out >= 100) return decision;
        }
        else render_text(10,100,add_newlines(text,300),100);

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}

void dialog(std::string file_name, SDL_Texture* tex)
{
    bool player_talking = true;
    drink* selected_drink = nullptr;

    std::fstream file;
    file.open(std::string("Data")+PATH_SEPARATOR+"Dialog"+PATH_SEPARATOR+file_name+".txt");
    std::string line;

    while (!file.eof())
    {
        std::getline(file,line);
        if (line.substr(0,2) == "P:") player_talking = true;
        else if (line.substr(0,2) == "B:") player_talking = false;
        else if (line.find('#') != std::string::npos)
        {
            if (!selected_drink)
            {
                drink *drink1 = random_drink(), *drink2 = random_drink();
                selected_drink = visual_novel(tex,drink1->name, drink2->name) ? drink2 : drink1;
            }

            auto sp = split(line, '#');

            visual_novel(tex,sp[0]+selected_drink->name+sp[1]);
        }
        else
        {
            visual_novel(tex,line);
        }
    }
    file.close();

    player->drunk_level += selected_drink->alcohol;
}

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
			    else if (e.key.keysym.sym == SDLK_e)
                {
                    for (Entrance* ent: entraces)
                    {
                        if (ent->collides(player)) ent->enter();
                    }
                }
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
