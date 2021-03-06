#include "creature.h"

Creature::Creature() { 
    x_pos = 0;
    y_pos = 0;
    jump_x = 0;
    jump_y = 0;
    old_row = 0;
    row = 0;
    old_col = 0;
    col = 0;
    start_jump_time = 0;
    bez_pts = new int*[4];
    jumping = false;
    for(int i = 0; i < 4; i++) {
        bez_pts[i] = new int[2];
    }
}

Creature::Creature(Board* in_board, SpriteSheet* in_sprites) {
    board = in_board;
    sprites = in_sprites;
}

Creature::~Creature() {
    for(int i = 0; i < 4; i++) {
        delete [] bez_pts[i];
    }
    delete [] bez_pts;
}

void Creature::animate(bool moving) {
    if(moving) {
        set_screen_pos();
    }
    SDL_Rect rect;
    rect.x = x_pos;
    rect.y = y_pos;
    rect.w = 100;
    rect.h = 100;
    sprites->draw(&rect);
}

void Creature::set_screen_pos() {
    //spawned = (y_pos > (board->get_y_mov() * board->get_board_len() * 2));
    if(!on_board()) {
        spawned = (y_pos < (board->get_y_mov() * board->get_board_len() * 2));
        y_pos += 10;
        x_pos = board->get_x_orig() + board->get_x_mov() * old_row - board->get_x_mov() * old_col;
    } else if(jumping) {   
        jump(); 
        x_pos = jump_x;
        y_pos = jump_y;
    } else {    
        x_pos = board->get_x_orig() + board->get_x_mov() * old_row - board->get_x_mov() * old_col;
        y_pos = board->get_y_orig() + board->get_y_mov() * old_row + board->get_y_mov() * old_col - board->get_orig_to_on_top();
    }
}

void Creature::move(int d_row, int d_col) {
    if(!jumping) {
        jump_sound->play();
        old_row = row;
        old_col = col;
        row += d_row;
        col += d_col;
        init_jump(d_row, d_col);
    }
}

//  Calibrates the jump animation
//  Assume jumping is true
void Creature::init_jump(int d_row, int d_col) {
    //  Set bezie points
    bez_pts[0][0] = x_pos;  bez_pts[0][1] = y_pos;
    int* last_point = board->get_cube_location(old_row + d_row, old_col + d_col);
    bez_pts[1][0] = 2 * x_pos/3 + last_point[0]/3;  bez_pts[1][1] = 2 * y_pos/3 + last_point[1]/3 - JUMP_HEIGHT;
    bez_pts[2][0] = x_pos/3 + 2 * last_point[0]/3;  bez_pts[2][1] = y_pos/3 + 2 * last_point[1]/3 - JUMP_HEIGHT;
    bez_pts[3][0] = last_point[0];  bez_pts[3][1] = last_point[1];

    jumping = true;
    delete [] last_point;
    start_jump_time = SDL_GetTicks();
}

//  Alters jump_x/y
//  Assume jumping is true
void Creature::jump() {
    float t = (float) (SDL_GetTicks() - start_jump_time) / JUMP_TIME;
    if(t < 1) {
        jump_x = pow((1-t),3) * bez_pts[0][0] + 3*pow((1-t),2) * t * bez_pts[1][0] + 3*(1-t)*pow(t,2) * bez_pts[2][0] + pow(t,3) * bez_pts[3][0];
		jump_y = pow((1-t),3) * bez_pts[0][1] + 3*pow((1-t),2) * t * bez_pts[1][1] + 3*(1-t)*pow(t,2) * bez_pts[2][1] + pow(t,3) * bez_pts[3][1];
    } else {
        old_row = row;
        old_col = col;
        jumping = false;
        start_jump_time = 0;
    }
}

void Creature::set_renderer(SDL_Renderer* in_renderer) {    renderer = in_renderer; }
void Creature::set_jump_sound(Sound* js) {  jump_sound = js;    }
int Creature::get_old_row() {   return old_row; }
int Creature::get_old_col() {   return old_col; }
int Creature::get_row() {   return row; }
int Creature::get_col() {   return col; }

int Creature::get_pos() {
    return 10 * row + col;
}

bool Creature::get_spawned() {  return spawned; }

bool Creature::on_board() {
    return  old_row >= 0 &&
            old_col >= 0 &&
            old_row + old_col < board->get_board_len();
}

void Creature::set_sprite_dir(int dir) {
    sprites->set_dir(dir);
}

int Creature::get_x_pos() { return x_pos;   }
int Creature::get_y_pos() { return y_pos;   }
SpriteSheet* Creature::get_sprites() {  return sprites; }