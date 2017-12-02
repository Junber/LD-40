#include "rendering.h"
#include "base.h"

#include <map>
#include <iostream>
#include <fstream>

SDL_Window* renderwindow;
SDL_Renderer* renderer;

const int window[2] = {320,180};
int zoom = 1;
bool fullscreen = false;

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

            if (sp[0] == "fullscreen") fullscreen = atoi(sp[1].c_str());
            else if (sp[0] == "zoom") zoom = atoi(sp[1].c_str());
        }
    }
}

void render_init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    load_options();

    SDL_DisplayMode current;
    SDL_GetDesktopDisplayMode(0, &current);

    renderwindow = SDL_CreateWindow("LD 40", 50, 50, fullscreen?current.w:window[0]*zoom, fullscreen?current.h:window[1]*zoom, SDL_WINDOW_SHOWN | (fullscreen&SDL_WINDOW_FULLSCREEN));
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderSetLogicalSize(renderer, window[0], window[1]);
}

std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s)) loaded_textures[s] = IMG_LoadTexture(renderer,(std::string("Data")+PATH_SEPARATOR+"Graphics"+PATH_SEPARATOR+s+".png").c_str());

    return loaded_textures[s];
}
