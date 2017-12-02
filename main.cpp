#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>

#ifndef _STATIC
void *__gxx_personality_v0;
#endif

const int window[2] = {500,500};

bool breakk = false;
SDL_Window* renderwindow;
SDL_Renderer* renderer;

int last_time;
float wait;
void limit_fps()
{
    wait = (100.0/6)-(SDL_GetTicks() - last_time);
    if (wait>0) SDL_Delay(wait);
    last_time = SDL_GetTicks();
}

template<class t> void remove_it(std::deque<t>* base, t thing)
{
    base->erase( std::remove( std::begin(*base), std::end(*base), thing ), std::end(*base) );
}

std::map<std::string,SDL_Texture*> loaded_textures;
SDL_Texture* load_image(std::string s)
{
    if (!loaded_textures.count(s)) loaded_textures[s] = IMG_LoadTexture(renderer,("Data\\Graphics\\"+s+".png").c_str());

    return loaded_textures[s];
}

class Object;
std::deque<Object*> objects;
class Object
{
    int corners[4][2], proj_min, proj_max;
public:
    int pos[2], size[2], rotation;
    SDL_Texture* tex;

    Object(int x, int y, std::string s)
    {
        pos[0] = x;
        pos[1] = y;

        rotation = 0;

        tex = load_image(s);
        SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);

        gen_corners();

        objects.push_back(this);
    }

    virtual ~Object()
    {
        remove_it(&objects, this);
    }

    virtual void update()
    {

    }

    void gen_corners()
    {
        const float angle = rotation*M_PI/180;
        for (int i=-1; i<=1; i+=2)
        {
            for (int u=-1; u<=1; u+=2)
            {
                corners[i+1+(u+1)/2][0] = i*size[0]/2*std::cos(angle) - u*size[1]/2*std::sin(angle)+pos[0];
                corners[i+1+(u+1)/2][1] = i*size[0]/2*std::sin(angle) + u*size[1]/2*std::cos(angle)+pos[1];

                //std::cout << "["<< i+1+(u+1)/2 << ": "<< corners[i+1+(u+1)/2][0] <<","<< corners[i+1+(u+1)/2][1] << "]";
            }
        }

        //std::cout << "\n";
    }

    void gen_proj_min_max(int axis_x, int axis_y)
    {
        for (int i = 0; i<4; ++i)
        {
            int proj = corners[i][0]*axis_x+corners[i][1]*axis_y;

            if (!i || proj < proj_min) proj_min = proj;
            if (!i || proj > proj_max) proj_max = proj;
        }
    }

    bool collides(Object* o)
    {
        int axiss[4][2] = {{   corners[1][0]-   corners[0][0],   corners[1][1]-   corners[0][1]},
                           {   corners[1][0]-   corners[3][0],   corners[1][1]-   corners[3][1]},
                           {o->corners[1][0]-o->corners[0][0],o->corners[1][1]-o->corners[0][1]},
                           {o->corners[1][0]-o->corners[3][0],o->corners[1][1]-o->corners[3][1]}};

        for (int i = 0; i<4; ++i)
        {
            gen_proj_min_max(axiss[i][0],axiss[i][1]);
            o->gen_proj_min_max(axiss[i][0],axiss[i][1]);

            //std::cout << i <<":"<< axiss[i][0] <<","<< axiss[i][1]<<": "<<proj_min <<","<< proj_max <<"  "<< o->proj_min <<","<< o->proj_max << "\n";

            if (proj_min > o->proj_max || proj_max < o->proj_min) return false;
        }

        return true;
    }

    virtual void render()
    {
        SDL_Rect r={pos[0]-size[0]/2, pos[1]-size[1]/2, size[0], size[1]};

        SDL_RenderCopyEx(renderer, tex, nullptr, &r, rotation, nullptr, SDL_FLIP_NONE);
    }
};

class Player: public Object
{
    const int movement_speed = 2;

public:
    Player(): Object(0,0,"Player") {}

    void update()
    {
        const Uint8* state = SDL_GetKeyboardState(nullptr);

        if (state[SDL_SCANCODE_D]) pos[0] += movement_speed;
        else if (state[SDL_SCANCODE_A]) pos[0] -= movement_speed;
        if (state[SDL_SCANCODE_S]) pos[1] += movement_speed;
        else if (state[SDL_SCANCODE_W]) pos[1] -= movement_speed;

        gen_corners();
    }
};

class Hitbox: public Object
{
public:
    Hitbox(int x, int y, int sx, int sy): Object(x,y,"")
    {
        size[0] = sx;
        size[1] = sy;
        gen_corners();
    }

    void update()
    {

    }

    void render() {}
};

int main(int argc, char* args[])
{
    IMG_Init(IMG_INIT_PNG);

    renderwindow = SDL_CreateWindow("LD 40", 50, 50, window[0], window[1], SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(renderwindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Player* player = new Player();

    Object* obj = new Object(20,20,"Player");

    //SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
    SDL_Event e;
	while (!breakk)
    {
        while(SDL_PollEvent(&e))
        {
			if (e.type == SDL_QUIT) breakk = true;

			else if (e.type == SDL_KEYDOWN)
			{
			    if (e.key.keysym.sym == SDLK_ESCAPE) breakk = true;
			    if (e.key.keysym.sym == SDLK_e) player->rotation+=45;
			}
        }

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        for (Object* o: objects)
        {
            o->update();
            o->render();
        }

        std::cout << player->collides(obj);

        SDL_RenderPresent(renderer);
        limit_fps();
    }

    return 0;
}
