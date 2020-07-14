#include "platform.h"

void Platform::set_bez_pts() {
    bez_pts[0][0] = x_pos; bez_pts[0][1] = y_pos;
    int* orig = board->get_cube_location(-1,-1);
    bez_pts[1][0] = 2 * x_pos/3 + orig[0]/3;  bez_pts[1][1] = 2 * y_pos/3 + orig[1]/3 - JUMP_HEIGHT;
    bez_pts[2][0] = x_pos/3 + 2 * orig[0]/3;  bez_pts[2][1] = y_pos/3 + 2 * orig[1]/3 - JUMP_HEIGHT;
    bez_pts[3][0] = orig[0];  bez_pts[3][1] = orig[1];

    delete [] orig;
    start_move_time = SDL_GetTicks();
}

void Platform::move() {
    float t = (float) (SDL_GetTicks() - start_move_time) / MOVE_TIME;
    if(t < 1) {
        x_pos = pow((1-t),3) * bez_pts[0][0] + 3*pow((1-t),2) * t * bez_pts[1][0] + 3*(1-t)*pow(t,2) * bez_pts[2][0] + pow(t,3) * bez_pts[3][0];
		y_pos = pow((1-t),3) * bez_pts[0][1] + 3*pow((1-t),2) * t * bez_pts[1][1] + 3*(1-t)*pow(t,2) * bez_pts[2][1] + pow(t,3) * bez_pts[3][1];
    } else {
        row = -1;
        col = -1;
        moving = false;
        start_move_time = 0;
    }
}

//  Rows and cols are set as random lengths
Platform::Platform(Board* in_board, SpriteSheet* in_sprites) {
    board = in_board;
    sprites = in_sprites;
    if(rand() % 2) {
        row = rand() % board->get_board_len();
        col = -1;
    } else {
        col = rand() % board->get_board_len();
        row = -1;
    }
    int* loc = board->get_cube_location(row, col);
    x_pos = loc[0];
    y_pos = loc[1];
    delete [] loc;

    bez_pts = new int*[4];
    for(int i = 0; i < 4; i++) {
        bez_pts[i] = new int[2];
    }
}

Platform::~Platform() {
    //delete board;
}

void Platform::set_loc(int r, int c) {
    row = r;
    col = c;
    x_pos = board->get_cube_location(row, col)[0];
    y_pos = board->get_cube_location(row, col)[1];
}

void Platform::animate() {
    if(start_move) {
        start_move_time = SDL_GetTicks();
        set_bez_pts();
        moving = true;
        start_move = false;
    }
    if(moving) {
        move();
    }
    SDL_Rect rect;
    rect.x = x_pos;
    rect.y = y_pos;
    rect.w = 100;
    rect.h = 100;
    sprites->draw(&rect);
}

int* Platform::get_pos() {
    int* pos = new int[2];
    pos[0] = x_pos;
    pos[1] = y_pos;
    return pos;
}

int Platform::get_row() { return row;   }
int Platform::get_col() { return col;   }
void Platform::start_moving() {    start_move = true;   }