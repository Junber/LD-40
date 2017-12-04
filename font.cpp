#include <map>
#include <deque>
#include <iostream>

#include "font.h"
#include "rendering.h"
#include "base.h"

TTF_Font* font;
void font_init()
{
    TTF_Init();
    font = TTF_OpenFont((std::string("Data")+PATH_SEPARATOR+"Fonts"+PATH_SEPARATOR+"Roboto-Light.ttf").c_str(),16);
}

std::map<std::pair<std::string,Uint8>,SDL_Texture*> loaded_texts;
SDL_Texture* text_to_texture(std::string s, Uint8 brightness)
{
    std::pair<std::string,Uint8> p = {s, brightness};
    if (!loaded_texts.count(p)) loaded_texts[p] = SDL_CreateTextureFromSurface(renderer,TTF_RenderUTF8_Solid(font, s.c_str(), {brightness,brightness,brightness,255}));
    return loaded_texts[p];
}

int add_newlines(std::string &s, int width)
{
    std::deque<std::string> splitt = split(s,' ');
    std::string ret = "", part="";
    int cur_width, height=0;
    for (std::string st: splitt)
    {
        if (st[0] == '\n')
        {
            ret += part+"-\n";
            part = st.substr(1);
            ++height;
        }
        else
        {
            TTF_SizeText(font,(part+" "+st).c_str(), &cur_width, nullptr);

            if (cur_width <= width)
                part += " "+st;
            else
            {
                ret += part+"\n";
                part = st;
                ++height;
            }
        }
    }

    ret += part;
    ++height;

    s = ret;
    return height*TTF_FontLineSkip(font);
}

void render_text(int posx, int posy, std::string s, Uint8 brightness, Uint8 alpha, bool r_aligned)
{
    int offset = 0;
    std::deque<std::string> splitt = split(s,'\n');
    for (auto st: splitt)
    {
        SDL_Texture* tex =text_to_texture(st, brightness);
        SDL_SetTextureAlphaMod(tex,alpha);
        int size[2];
        SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);
        SDL_Rect r = {posx-r_aligned*size[0], posy+offset, size[0], size[1]};
        SDL_RenderCopy(renderer, tex, nullptr, &r);

        offset += TTF_FontLineSkip(font);
    }
}
