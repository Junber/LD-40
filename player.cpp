#include "player.h"
#include "rendering.h"

Player* player;

Player::Player(): Object(0,window[1]/2,"walk1",true)
{
    hitbox_size[1] = 6;
    hitbox_offset[1] = 18;

    gen_corners();

    update_camera();
}

void Player::update_camera()
{
    camera[0] = std::max(-300,pos[0]-window[0]/2);
}

void Player::update(bool increase_anim_time)
{
    const Uint8* state = SDL_GetKeyboardState(nullptr);

    int last_pos[2] = {pos[0], pos[1]};

    if (state[SDL_SCANCODE_D]) pos[0] += movement_speed;
    else if (state[SDL_SCANCODE_A]) pos[0] -= movement_speed;
    if (state[SDL_SCANCODE_S]) pos[1] += movement_speed;
    else if (state[SDL_SCANCODE_W]) pos[1] -= movement_speed;

    if (last_pos[0] != pos[0]  || last_pos[1] != pos[1])
    {
        //rotation = std::atan2(pos[1]-last_pos[1], pos[0]-last_pos[0])*180/M_PI;
        gen_corners();
        cur_anim_time++;

        if (pos[0] < -292) pos[0] = -292;
        if (pos[1] < 70) pos[1] = 70;
        else if (pos[1] > 156) pos[1] = 156;

        update_camera();
    }

    if (last_pos[0] != pos[0]) flipped = last_pos[0] > pos[0];

    Object::update(false);
}
