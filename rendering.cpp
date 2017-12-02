#include "rendering.h"

#include <map>

SDL_Window* renderwindow;
SDL_Renderer* renderer;

const int window[2] = {500,500};

void render_init()
{
    IMG_Init(IMG_INIT_PNG);

    renderwindow = SDL_CreateWindow("LD 40", 50, 50, window[0], window[1], SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s)) loaded_textures[s] = IMG_LoadTexture(renderer,("Data\\Graphics\\"+s+".png").c_str());

    return loaded_textures[s];
}
