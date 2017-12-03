#ifndef _OBJECT
#define _OBJECT

#include <deque>
#include <algorithm>
#include <SDL_image.h>
#include <string>

typedef std::pair<SDL_Texture*, std::deque<int> > animation;

extern int camera[2];

template<class t> void remove_it(std::deque<t>* base, t thing)
{
    base->erase( std::remove( std::begin(*base), std::end(*base), thing ), std::end(*base) );
}

class Object
{
    int corners[4][2], proj_min, proj_max;
public:
    int pos[2], size[2], rotation, hitbox_size[2], hitbox_offset[2];
    bool flipped;

    animation* anim;
    int cur_anim_frame, cur_anim_time;

    Object(int x, int y, std::string s, bool load_as_animation=false);
    virtual ~Object();

    void load_animation(std::string s);

    virtual void update(bool increase_anim_time=true);
    virtual void render();

    void gen_corners();
    void gen_proj_min_max(int axis_x, int axis_y);
    bool collides(Object* o);
};

extern std::deque<Object*> objects;

#endif // _OBJECT
