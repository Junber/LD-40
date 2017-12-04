#include "rendering.h"
#include "base.h"
#include "object.h"
#include "background.h"
#include "player.h"

#include <map>
#include <iostream>
#include <fstream>

SDL_Window* renderwindow;
SDL_Renderer* renderer;

const int window[2] = {320,180};
int zoom = 1, fullscreen = false;

SDL_Texture* beermeter_tex;
int beermeter_w, beermeter_h;

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

    renderwindow = SDL_CreateWindow("LD 40", 50, 50, fullscreen?current.w:window[0]*zoom, fullscreen?current.h:window[1]*zoom, SDL_WINDOW_SHOWN | (fullscreen?SDL_WINDOW_FULLSCREEN_DESKTOP:0));
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetLogicalSize(renderer, window[0], window[1]);

    beermeter_tex = load_image("beermeter");
    SDL_QueryTexture(beermeter_tex,nullptr,nullptr,&beermeter_w,&beermeter_h);
    beermeter_h /= 15;
}

void render_init_update()
{
    SDL_SetWindowFullscreen(renderwindow,fullscreen?SDL_WINDOW_FULLSCREEN_DESKTOP:0);

    SDL_DisplayMode current;
    SDL_GetDesktopDisplayMode(0, &current);

    SDL_SetWindowSize(renderwindow,fullscreen?current.w:window[0]*zoom, fullscreen?current.h:window[1]*zoom);
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

        if (blick_progress < 10/(1+drunkenness::fast_blinking))
        {
            SDL_Rect r = {0,0,window[0],blick_progress*window[1]/10};
            SDL_RenderFillRect(renderer,&r);

            r.y = window[1]-r.h;
            SDL_RenderFillRect(renderer,&r);
        }
        else if (blick_progress < 40/(1+drunkenness::fast_blinking))
        {
            SDL_RenderClear(renderer);
        }
        else if (blick_progress < 50/(1+drunkenness::fast_blinking))
        {
            SDL_Rect r = {0,0,window[0],(50/(1+drunkenness::fast_blinking)-blick_progress)*window[1]/10};
            SDL_RenderFillRect(renderer,&r);

            r.y = window[1]-r.h;
            SDL_RenderFillRect(renderer,&r);
        }
        else blick_progress = -1;
    }
    else if (!random(0,drunkenness::blick_frequency))
    {
        ++blick_progress;
        if (drunkenness::random_keys) player->assign_keys();
    }
}

bool sort_criteria(Object* a, Object* b)
{
    return a->pos[1]+a->size[1]/2 < b->pos[1]+b->size[1]/2;
}

// TODO (Junber#1#): Add drunkenness UI
void render_everything(bool blink)
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

    if (drunkenness::see_ui)
    {
        SDL_Rect src = {0,player->alcohol_points*beermeter_h,beermeter_w,beermeter_h}, dest = {5,5,beermeter_w,beermeter_h};
        SDL_RenderCopy(renderer, beermeter_tex, &src, &dest);
    }

    if (blink) blinking();

    SDL_RenderPresent(renderer);
}
