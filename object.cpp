#include "object.h"
#include "rendering.h"

std::deque<Object*> objects;

Object::Object(int x, int y, std::string s)
{
    pos[0] = x;
    pos[1] = y;

    rotation = 0;

    tex = load_image(s);
    SDL_QueryTexture(tex, nullptr, nullptr, &size[0], &size[1]);

    gen_corners();

    objects.push_back(this);
}

Object::~Object()
{
    remove_it(&objects, this);
}

void Object::render()
{
    SDL_Rect r={pos[0]-size[0]/2, pos[1]-size[1]/2, size[0], size[1]};

    SDL_RenderCopyEx(renderer, tex, nullptr, &r, rotation, nullptr, SDL_FLIP_NONE);
}

void Object::gen_corners()
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
