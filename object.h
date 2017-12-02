#ifndef _OBJECT
#define _OBJECT

#include <deque>
#include <algorithm>
#include <SDL_image.h>

template<class t> void remove_it(std::deque<t>* base, t thing)
{
    base->erase( std::remove( std::begin(*base), std::end(*base), thing ), std::end(*base) );
}

class Object
{
    int corners[4][2], proj_min, proj_max;
public:
    int pos[2], size[2], rotation;
    SDL_Texture* tex;

    Object(int x, int y, std::string s);
    virtual ~Object();

    virtual void update() {}
    virtual void render();

    void gen_corners();
    void gen_proj_min_max(int axis_x, int axis_y);
    bool collides(Object* o);
};

extern std::deque<Object*> objects;

#endif // _OBJECT
