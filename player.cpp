#include "player.h"
#include "rendering.h"
#include "base.h"

#include <string>
#include <iostream>

Player* player;

int w,a,s,d;
void Player::assign_keys()
{
    if (drunkenness::random_keys)
    {
        std::deque<int> keys = {SDL_SCANCODE_W,SDL_SCANCODE_A,SDL_SCANCODE_S,SDL_SCANCODE_D};

        w = keys[random(0,keys.size()-1)];
        remove_it(&keys,w);
        a = keys[random(0,keys.size()-1)];
        remove_it(&keys,a);
        s = keys[random(0,keys.size()-1)];
        remove_it(&keys,s);
        d = keys[random(0,keys.size()-1)];
        //std::cout << w <<" "<< a <<" "<< s <<" "<< d <<"\n";
    }
    else
    {
        w = SDL_SCANCODE_W;
        a = SDL_SCANCODE_A;
        s = SDL_SCANCODE_S;
        d = SDL_SCANCODE_D;
    }
}

Player::Player(): Object(0,window[1]/2,"walk1",true)
{
    drunk_level = 1;
    alcohol_points = 0;
    change_movement(drunkenness::auto_running?auto_runner:in_control);

    hitbox_size[0] = 9;
    hitbox_size[1] = 3;
    hitbox_offset[1] = 18;

    save_pos();
    gen_corners();
    update_camera();

    assign_keys();
}

void Player::update_camera()
{
    camera[0] = std::max(-300,pos[0]-window[0]/2);
}

void Player::update(bool increase_anim_time)
{
    if (cur_movement == in_control || cur_movement == auto_runner)
    {
        if (drunkenness::movement_speed > 0 || time_till_move <= 0)
        {
            const Uint8* state = SDL_GetKeyboardState(nullptr);

            int last_pos[2] = {pos[0], pos[1]};

            if (cur_movement == auto_runner)
            {
                pos[0] += (drunkenness::movement_speed>0?drunkenness::movement_speed:1);
                if (state[d] && camera_x_offset <  window[0]/2-size[0])
                {
                    camera_x_offset++;
                    pos[0]++;
                }
                else if (state[a] && camera_x_offset > -window[0]/2+size[0])
                {
                    camera_x_offset--;
                    pos[0]--;
                }
            }
            else
            {
                if (state[d])      pos[0] += (drunkenness::movement_speed>0?drunkenness::movement_speed:1);
                else if (state[a]) pos[0] -= (drunkenness::movement_speed>0?drunkenness::movement_speed:1);
            }

            if (state[s])      pos[1] += (drunkenness::movement_speed>0?drunkenness::movement_speed:1);
            else if (state[w]) pos[1] -= (drunkenness::movement_speed>0?drunkenness::movement_speed:1);

            if (last_pos[0] != pos[0]  || last_pos[1] != pos[1])
            {
                if (drunkenness::movement_speed < 0) time_till_move = -drunkenness::movement_speed;

                //rotation = std::atan2(pos[1]-last_pos[1], pos[0]-last_pos[0])*180/M_PI;
                gen_corners();
                cur_anim_time += abs(pos[1]-last_pos[1]) + abs(pos[0]-last_pos[0]);

                if (pos[0] < -292) pos[0] = -292;
                if (pos[1] < 70) pos[1] = 70;
                else if (pos[1] > 156) pos[1] = 156;
            }

            update_camera();

            if (last_pos[0] != pos[0]) flipped = last_pos[0] > pos[0];
        }
        else
        {
            time_till_move--;
        }
    }
    else if (cur_movement == stumble)
    {
        cur_anim_time++;
        if (cur_anim_time >= anim->second[cur_anim_frame])
        {
            if (cur_anim_frame >= anim->second.size()-1)
            {
                change_movement(drunkenness::auto_running?auto_runner:in_control);
                pos[0] += 5*(flipped?-1:1);
                pos[1]--;
                update_camera();
            }
            else if (cur_anim_frame == 1 || cur_anim_frame == 3) pos[0] += 2*(flipped?-1:1);
            else if (cur_anim_frame == 2) pos[0] -= 2*(flipped?-1:1);
            else if (cur_anim_frame == 4) pos[0] += 3*(flipped?-1:1);

            gen_corners();
        }
    }
    else if (cur_movement == sway)
    {
        cur_anim_time++;
        if (cur_anim_time >= anim->second[cur_anim_frame])
        {
            if (cur_anim_frame >= anim->second.size()-1)
            {
                change_movement(drunkenness::auto_running?auto_runner:in_control);
                player->cur_anim_frame=2;

                if (drunk_level==4) pos[0] -= 4;
                else pos[1] -= 2;

                update_camera();
            }

            gen_corners();
        }
    }
    else if (cur_movement == walking_up)
    {
        cur_anim_time++;
        pos[1] -= drunkenness::movement_speed;
        gen_corners();
    }

    Object::update(false);
}

bool Player::is_in_control()
{
    return cur_movement == in_control || cur_movement == auto_runner;
}

void Player::change_movement(player_movement m)
{
    cur_movement = m;
    if (m == in_control) load_animation("walk"+std::to_string(drunk_level));
    if (m == sway)
    {
        load_animation("walk4_shuffle");
        if (drunk_level==4) pos[0] += 4;
        else pos[1] += 2;
    }
    else if (m == stumble)
    {
        load_animation("walk4_stumble");
        pos[0] += 5*(flipped?-1:1);
    }

    if (cur_movement != walking_up) cur_anim_frame = cur_anim_time = 0;
}

void Player::save_pos()
{
    saved_pos[0] = pos[0];
    saved_pos[1] = pos[1];
}

// TODO (Junber#1#): Add "death" screen
void Player::kill()
{
    pos[0] = saved_pos[0];
    pos[1] = saved_pos[1];
    camera_x_offset = 0;

    delete_all_the_shit = true;
}

void Player::drink(int alcohol)
{
    alcohol_points += alcohol;
    if (alcohol_points >= 15)
    {
        ++drunk_level;
        alcohol_points -= 15;

        if (drunk_level == 2)
        {
            drunkenness::blick_frequency = 1200;
            drunkenness::base_movement_speed = 1;
        }
        else if (drunk_level == 3)
        {
            drunkenness::auto_running = false;
            camera_x_offset = 0;
            drunkenness::blick_frequency = 800;
        }
        else if (drunk_level == 4)
        {
            drunkenness::blur = 30;
            drunkenness::base_movement_speed = -2;
            drunkenness::swaying = 1800;
            drunkenness::blick_frequency = 500;
        }
        else if (drunk_level == 5)
        {
            drunkenness::blur = 10;
            drunkenness::base_movement_speed = -3;
            drunkenness::random_keys = true;
            drunkenness::swaying = 900;
        }
        else
        {
// TODO (Junber#1#): Add win screen
            breakk = true;
        }

        drunkenness::movement_speed = drunkenness::base_movement_speed;
        change_movement(drunkenness::auto_running?auto_runner:in_control);
    }
}
