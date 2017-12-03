#include "rendering.h"
#include "base.h"
#include "object.h"
#include "background.h"

#include <map>
#include <iostream>
#include <fstream>

SDL_Window* renderwindow;
SDL_Renderer* renderer;

const int window[2] = {320,180};
int zoom = 1;
bool fullscreen = false;

void load_option_rendering(std::string s, std::string s2)
{
    if (s == "fullscreen") fullscreen = atoi(s2.c_str());
    else if (s == "zoom") zoom = atoi(s2.c_str());
}

void render_init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    SDL_DisplayMode current;
    SDL_GetDesktopDisplayMode(0, &current);

    renderwindow = SDL_CreateWindow("LD 40", 50, 50, fullscreen?current.w:window[0]*zoom, fullscreen?current.h:window[1]*zoom, SDL_WINDOW_SHOWN | (fullscreen&SDL_WINDOW_FULLSCREEN));
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetLogicalSize(renderer, window[0], window[1]);
}

std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s)) loaded_textures[s] = IMG_LoadTexture(renderer,(std::string("Data")+PATH_SEPARATOR+"Graphics"+PATH_SEPARATOR+s+".png").c_str());

    return loaded_textures[s];
}

int blick_progress = -1;
void blinking()
{
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
}

bool sort_criteria(Object* a, Object* b)
{
    return a->pos[1]+a->hitbox_offset[1] < b->pos[1]+b->hitbox_offset[1];
}

void render()
{
    for (Background* o: backgrounds)
    {
        o->render();
    }
    std::stable_sort(objects.begin(), objects.end(), sort_criteria);
    for (Object* o: objects)
    {
        o->render();
    }

    blinking();
}
