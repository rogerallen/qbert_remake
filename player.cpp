#include <iostream>
#include "player.h"
using namespace std;

Player::Player(Board* in_board, SpriteSheet* in_sprites) {
    spawned = true;
    alive = true;
    JUMP_TIME = 500;
    row = 0;
    col = 0;
    board = in_board;
    sprites = in_sprites;
    platforms = nullptr;
    num_platforms = -1;
    enemies = nullptr;
}

void Player::set_coll_sound(Sound* in_s) {
    coll_sound = in_s;
}

void Player::set_fall_sound(Sound* in_s) {
    fall_sound = in_s;
}

void Player::set_enemies(Creature** in_enemies, int in_num_enemies) {
    enemies = in_enemies;
    num_enemies = in_num_enemies;
}

void Player::set_platforms(Platform** in_platforms, int num_plats) {
    platforms = in_platforms;
    num_platforms = num_plats;
}

void Player::spawn() {
    row = 0;
    col = 0;
}

void Player::jump() {
    Creature::jump();
    if(!jumping && Creature::on_board()) {  board->update_color(row, col);  }
}

void Player::animate(bool moving) {
    if(!on_board()) {   
        if(alive) {
            if(!transporting) {
                for(int i = 0; i < num_platforms; i++) {
                    if(row == platforms[i]->get_row() && col == platforms[i]->get_col()) {
                        transporting = true;
                        plat_id = i;
                        platforms[plat_id]->start_moving();
                    }
                }
            }
            if(transporting) {
                x_pos = platforms[plat_id]->get_x_pos();
                y_pos = platforms[plat_id]->get_y_pos();
                SDL_Rect rect;
                rect.x = x_pos;
                rect.y = y_pos;
                rect.w = 100;
                rect.h = 100;
                sprites->draw(&rect);

                transporting = platforms[plat_id]->get_moving() || platforms[plat_id]->get_start_moving();
                
                if(!transporting) {
                    old_row = 0;
                    old_col = 0;
                    row = 0;
                    col = 0;
                    board->update_color(0,0);
                }
            } else {
                fall_sound->play();
                alive = false;  
                Creature::animate(moving);
            }
        } else {
            Creature::animate(moving);
        }
    }
    if(alive) {
        if(!transporting) {
            Creature::animate(moving);
        }
        for(int i = 0; i < num_enemies; i++) {
            if( enemies[i]->get_x_pos() <= x_pos && 
                enemies[i]->get_x_pos() + 50 >= x_pos &&
                enemies[i]->get_y_pos() <= y_pos &&
                enemies[i]->get_y_pos() + 50 >= y_pos) {
                alive = false;
                coll_sound->play();
            }
        }
    } else {    
    }
}

void Player::set_screen_pos() {
    if(!transporting) {
        Creature::set_screen_pos();
    }
}

bool Player::get_alive() {  return alive;   }
