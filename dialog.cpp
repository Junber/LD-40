#include "dialog.h"
#include "base.h"
#include "rendering.h"
#include "font.h"
#include "player.h"

#include <fstream>
#include <deque>

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

            render_text(10 +(decision?0:move_out)/2,100+(decision?move_out:0),add_newlines(text ,145),0,std::max(0,255-7*(decision?move_out:0)));
            render_text(155-(decision?move_out:0)/2,100+(decision?0:move_out),add_newlines(text2,145),0,std::max(0,255-7*(decision?0:move_out)));

            if (move_out >= 100) return decision;
        }
        else render_text(10,100,add_newlines(text,300),0);

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
