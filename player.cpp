#include "player.h"
#include "rendering.h"
#include <string>
Player* player;

Player::Player(): Object(0,window[1]/2,"walk1",true)
{
    drunk_level = 1;

    cur_movement = in_control;

    hitbox_size[0] = 9;
    hitbox_size[1] = 3;
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
    if (cur_movement == in_control)
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
    }
    else if (cur_movement == stumble)
    {
        cur_anim_time++;
        if (cur_anim_time >= anim->second[cur_anim_frame])
        {
            if (cur_anim_frame >= anim->second.size()-1)
            {
                change_movement(in_control);
                pos[0] += 5;
                pos[1]--;
                update_camera();
            }
            else if (cur_anim_frame == 1 || cur_anim_frame == 3) pos[0] += 2;
            else if (cur_anim_frame == 2) pos[0] -= 2;
            else if (cur_anim_frame == 4) pos[0] += 3;
        }
    }

    Object::update(false);
}

void Player::change_movement(player_movement m)
{
    cur_movement = m;
    if (m == in_control) load_animation("walk"+std::to_string(drunk_level));
    else if (m == stumble)
    {
        load_animation("walk4_stumble");
        pos[0] -= 2;
    }

    cur_anim_frame = cur_anim_time = 0;
}
