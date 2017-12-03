#include "object.h"
#include "rendering.h"
#include "base.h"

#include <iostream>
#include <fstream>
#include <map>

std::deque<Object*> objects;

int camera[2] = {0,0};
int camera_x_offset = 0;

Object::Object(int x, int y, std::string s, bool load_as_animation)
{
    pos[0] = x;
    pos[1] = y;

    rotation = 0;

    if (load_as_animation) load_animation(s);
    else
    {
        anim->first = load_image(s);
        anim->second.push_back(-1);
        cur_anim_frame = cur_anim_time = 0;

        SDL_QueryTexture(anim->first, nullptr, nullptr, &size[0], &size[1]);
    }

    hitbox_size[0] = size[0];
    hitbox_size[1] = size[1];
    hitbox_offset[0] = 0;
    hitbox_offset[1] = 0;

    flipped = false;

    gen_corners();

    objects.push_back(this);
}

Object::~Object()
{
    remove_it(&objects, this);
}

std::map<std::string, animation*> loaded_animations;
void Object::load_animation(std::string s)
{
    if (loaded_animations.count(s)) anim = loaded_animations[s];
    else
    {
        anim = new animation();
        anim->first = load_image(s);
        anim->second = std::deque<int>();

        std::fstream file;
        file.open(std::string("Data")+PATH_SEPARATOR+"Timing"+PATH_SEPARATOR+s+".txt");
        std::string line;
        while (!file.eof())
        {
            std::getline(file,line);
            auto sp = split(line,'x');
            for (int i=0; i<std::atoi(sp[1].c_str()); ++i) anim->second.push_back(std::atoi(sp[0].c_str()));
        }

        file.close();
        loaded_animations[s] = anim;
    }

    SDL_QueryTexture(anim->first, nullptr, nullptr, &size[0], &size[1]);

    size[1] /= anim->second.size();

    cur_anim_frame = cur_anim_time = 0;
}

void Object::update(bool increase_anim_time)
{
    if (anim->second[cur_anim_frame] > 0)
    {
        if (increase_anim_time) ++cur_anim_time;

        if (cur_anim_time >= anim->second[cur_anim_frame])
        {
            ++cur_anim_frame;
            cur_anim_frame %= anim->second.size();
            cur_anim_time=0;
        }
    }
}

void Object::render()
{
    SDL_Rect dest={pos[0]-size[0]/2-camera[0]+camera_x_offset, pos[1]-size[1]/2-camera[1], size[0], size[1]},
                src = {0, size[1]*cur_anim_frame, size[0], size[1]};

    SDL_SetTextureAlphaMod(anim->first,drunkenness::blur);

    SDL_RenderCopyEx(renderer, anim->first, &src, &dest, rotation, nullptr, flipped?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE);
}

void Object::gen_corners()
{
    const float angle = rotation*M_PI/180;
    for (int i=-1; i<=1; i+=2)
    {
        for (int u=-1; u<=1; u+=2)
        {
            corners[i+1+(u+1)/2][0] = i*hitbox_size[0]/2*std::cos(angle) - u*hitbox_size[1]/2*std::sin(angle)+pos[0]+hitbox_offset[0];
            corners[i+1+(u+1)/2][1] = i*hitbox_size[0]/2*std::sin(angle) + u*hitbox_size[1]/2*std::cos(angle)+pos[1]+hitbox_offset[1];

            //std::cout << "["<< i+1+(u+1)/2 << ": "<< corners[i+1+(u+1)/2][0] <<","<< corners[i+1+(u+1)/2][1] << "]";
        }
    }

    //std::cout << "\n";
}

void Object::gen_proj_min_max(int axis_x, int axis_y)
{
    for (int i = 0; i<4; ++i)
    {
        int proj = corners[i][0]*axis_x+corners[i][1]*axis_y;

        if (!i || proj < proj_min) proj_min = proj;
        if (!i || proj > proj_max) proj_max = proj;
    }
}

bool Object::collides(Object* o)
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
