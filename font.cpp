#include <map>
#include <deque>
#include <iostream>

#include "font.h"
#include "rendering.h"

std::deque<std::string> split(std::string s, char seperator)
{
    std::deque<std::string> ret;
    ret.push_back("");
    for (char c: s)
    {
        if (c == seperator)
        {
            ret.push_back("");
        }
        else
        {
            ret[ret.size()-1] += c;
        }
    }

    return ret;
}

TTF_Font* font;
void font_init()
{
    TTF_Init();
    font = TTF_OpenFont((std::string("Data")+PATH_SEPARATOR+"Fonts"+PATH_SEPARATOR+"RTRaleway-Regular.ttf").c_str(),16);
}

std::map<std::pair<std::string,Uint8>,SDL_Texture*> loaded_texts;
SDL_Texture* text_to_texture(std::string s, Uint8 brightness)
{
    std::pair<std::string,Uint8> p = {s, brightness};
    if (!loaded_texts.count(p)) loaded_texts[p] = SDL_CreateTextureFromSurface(renderer,TTF_RenderText_Solid(font, s.c_str(), {brightness,brightness,brightness,255}));
    return loaded_texts[p];
}

std::string add_newlines(std::string s, int width)
{
    std::deque<std::string> splitt = split(s,' ');
    std::string ret = "", part="";
    int cur_width;
    for (std::string st: splitt)
    {
        TTF_SizeText(font,(part+" "+st).c_str(), &cur_width, nullptr);

        if (cur_width <= width)
            part += " "+st;
        else
        {
            ret += "\n"+part;
            part = st;
        }
    }

    if (!part.empty()) ret += "\n"+part;
    return ret;
}

void render_text(int posx, int posy, std::string s, Uint8 brightness)
{
    int offset = 0;
    std::deque<std::string> splitt = split(s,'\n');
    for (auto st: splitt)
    {
        SDL_Texture* tex =text_to_texture(st, brightness);
        int size[2];
        SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);
        SDL_Rect r = {posx, posy+offset, size[0], size[1]};
        SDL_RenderCopy(renderer, tex, nullptr, &r);

        offset += TTF_FontLineSkip(font);
    }
}
