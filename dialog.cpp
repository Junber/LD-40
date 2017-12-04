#include "dialog.h"
#include "base.h"
#include "rendering.h"
#include "font.h"
#include "player.h"
#include "sound.h"

#include <fstream>
#include <deque>
#include <iostream>

struct drink
{
    std::string name_uncut, name_cut;
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
        d->alcohol = std::stoi(sp[1]);

        auto spl = split(sp[0],'-');
        if (spl.size() > 1)
        {
            d->name_uncut = spl[0]+spl[1];
            d->name_cut = spl[0]+" \n"+spl[1];
        }
        else d->name_uncut = d->name_cut = sp[0];

        drinks.push_back(d);
    }
    file.close();
}

drink* random_drink()
{
    drink* d = drinks[random(0,drinks.size()-1)];
    remove_it(&drinks,d);
    return d;
}

#define option_num 5
void options()
{
    std::string text[option_num] = {"Fullscreen","Zoom","SFX Volume","Music Volume","Exit"};
    int *value[option_num] = {&fullscreen, &zoom, &sfx_volume, &music_volume, nullptr};
    int range_max[option_num] = {1,9,128,128,0};
    int range_min[option_num] = {0,1,0,0,0};
    int pointer = 0;

    int w,h;
    SDL_Texture* pointer_tex = load_image("pointer");
    SDL_QueryTexture(pointer_tex,nullptr,nullptr,&w,&h);


    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    if (e.key.keysym.sym == SDLK_ESCAPE) return;
			    else if (e.key.keysym.sym == SDLK_e || e.key.keysym.sym == SDLK_a || e.key.keysym.sym == SDLK_d)
                {
                    if (value[pointer])
                    {
                        if (e.key.keysym.sym == SDLK_a) --(*value[pointer]);
                        else                            ++(*value[pointer]);;

                        if (*value[pointer] > range_max[pointer]) *value[pointer] = range_min[pointer];
                        if (*value[pointer] < range_min[pointer]) *value[pointer] = range_max[pointer];

                        render_init_update();
                        sound_init_update();
                    }
                    else if (e.key.keysym.sym == SDLK_e) breakk = true;
                }
			    else if (e.key.keysym.sym == SDLK_w)
                {
                    pointer--;
                    if (pointer==1 && *value[0]) pointer--;

                    if (pointer<0) pointer = option_num-1;
                }
			    else if (e.key.keysym.sym == SDLK_s)
                {
                    pointer++;
                    if (pointer==1 && *value[0]) pointer++;

                    if (pointer>=option_num) pointer = 0;
                }
			}
        }

        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderClear(renderer);

        for (int i=0; i<option_num;i++)
        {
            render_text(50,50+i*20,text[i]+(value[i]?": "+std::to_string(*value[i]):""),(i==1 && *value[0])?100:255);
        }

        SDL_Rect r = {45-w,58+pointer*20,w,h};
        SDL_RenderCopy(renderer,pointer_tex,nullptr,&r);

        SDL_RenderPresent(renderer);
        limit_fps();
    }
}

bool visual_novel(SDL_Texture* tex, std::string text, std::string text2, bool player)
{
    bool choice = !text2.empty(), textbox = !text.empty(), decision=false;
    SDL_Texture *top = load_image("top_"+std::string(player?"play":"bar")), *bottom = load_image("bottom_"+std::string(player?"play":"bar")),
        *middle = load_image("middle");

    int height = add_newlines(text ,choice?88:158);
    if (choice) height = std::max(height,add_newlines(text2,88));

    int move_out = 0;

    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    if (e.key.keysym.sym == SDLK_ESCAPE) options();
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

        if (textbox)
        {
            //std::cout << "." << text <<  << "\n";
            SDL_Rect r = {12,20,208,20};
            SDL_RenderCopy(renderer,top,nullptr,&r);
            while (r.y <= height-16)
            {
                r.y += 20;
                SDL_RenderCopy(renderer,middle,nullptr,&r);
            }
            r.y += 20;
            SDL_RenderCopy(renderer,bottom,nullptr,&r);

            render_text(37 +(decision?0:move_out)/2,25+(decision?move_out:0),text ,0,std::max(0,255-7*(decision?move_out:0)));
            if (choice)
            {
                if (move_out>0) move_out++;

                render_text(193-(decision?move_out:0)/2,25+(decision?0:move_out),text2,0,std::max(0,255-7*(decision?0:move_out)), true);

                if (move_out >= 100) return decision;
            }
        }

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
                selected_drink = visual_novel(tex,drink1->name_cut, drink2->name_cut, player_talking) ? drink2 : drink1;
            }

            auto sp = split(line, '#');

            visual_novel(tex,sp[0]+selected_drink->name_uncut+sp[1],"",player_talking);
        }
        else
        {
            if (line.substr(0,6) == "*Gulp*") play_sound(load_sound("drink"));
            visual_novel(tex,line,"",player_talking);
        }
    }
    file.close();

    player->drink(selected_drink->alcohol);
}
