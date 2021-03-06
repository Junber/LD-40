#include "player.h"
#include "rendering.h"
#include "base.h"
#include "dialog.h"
#include "sound.h"

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
    time_to_puke=-1;
    drunk_level = 1;
    alcohol_points = 0;
    dialog_this_level=0;
    hits = 0;
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
    else if (cur_movement == collide)
    {
        cur_anim_time++;
        if (cur_anim_time >= anim->second[cur_anim_frame] && cur_anim_frame >= anim->second.size()-1)
        {
            change_movement(drunkenness::auto_running?auto_runner:in_control);
        }
    }
    else if (cur_movement == revive)
    {
        cur_anim_time++;
        if (cur_anim_time >= anim->second[cur_anim_frame] && cur_anim_frame >= anim->second.size()-1)
        {
            change_movement(drunkenness::auto_running?auto_runner:in_control);
			pos[0] -= 15;

            gen_corners();
        }
    }
    else if (cur_movement == fall)
    {
        cur_anim_time++;
        if (cur_anim_time >= anim->second[cur_anim_frame] && cur_anim_frame >= anim->second.size()-1)
        {
            change_movement(revive);
            pos[0] += 15;
            pos[1] += 3;
            gen_corners();
        }
    }
    else if (cur_movement == die)
    {
        cur_anim_time++;
        if (cur_anim_time >= anim->second[cur_anim_frame] && cur_anim_frame >= anim->second.size()-1)
        {
            kill();
            update();
            return;
        }
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
    if (m == in_control || m == auto_runner) load_animation("walk"+std::to_string(drunk_level));
    if (m == fall || m == die) load_animation("death");
    if (m == revive) load_animation("revive");
    else if (m == collide) load_animation("collide");
    else if (m == puking) load_animation("puke");
    else if (m == sway)
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

    hits = 0;
}

void Player::kill()
{
    pos[0] = saved_pos[0];
    pos[1] = saved_pos[1];
    camera_x_offset = 0;
    hits = 0;

    change_movement(drunkenness::auto_running?auto_runner:in_control);

    visual_novel(load_image("endscreen"),"","",false);

    delete_all_the_shit = true;
}

void Player::puke()
{
    change_movement(puking);
    cur_anim_time++;

    int sum = 0;
    for (int i: anim->second) sum += i;
    int blur_per_frame = (255-drunkenness::blur)/sum;

    while (cur_anim_frame || cur_anim_time)
    {
        SDL_FlushEvents(SDL_FIRSTEVENT,SDL_LASTEVENT);

        Object::update(true);
        drunkenness::blur += blur_per_frame;

        if (cur_anim_frame==5 && !cur_anim_time) play_sound(load_sound("vomit"));

        render_everything(false);
        limit_fps();
    }

    visual_novel(load_image("winscreen"),"","",false);
    breakk = true;
}

void Player::drink(int alcohol)
{
    alcohol_points += alcohol;
    if (alcohol_points >= 15)
    {
        ++drunk_level;
        dialog_this_level=0;
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
            drunkenness::blick_frequency = 1000;
            drunkenness::fast_blinking = false;
            drunkenness::mailbox_monster = true;
        }
        else if (drunk_level == 4)
        {
            drunkenness::blur = 40;
            drunkenness::base_movement_speed = -2;
            drunkenness::swaying = 1800;
            drunkenness::blick_frequency = 800;
            drunkenness::see_ui = false;
            drunkenness::flamingo_people = true;
            play_music(load_music("Barjazz2"));
        }
        else if (drunk_level == 5)
        {
            drunkenness::blur = 10;
            drunkenness::base_movement_speed = -3;
            drunkenness::random_keys = true;
            drunkenness::swaying = 1200;
        }
        else
        {
            drunk_level--;
            alcohol_points = 15;
            time_to_puke=300;
        }

        drunkenness::movement_speed = drunkenness::base_movement_speed;
        change_movement(drunkenness::auto_running?auto_runner:in_control);
    }
}
