#include "game.h"

void keyEvent(Player* player, bool& got_quit_event, SDL_Event& event, Sound** sounds);

Game::Game() {  
    sprites = nullptr;
    snake_timer_start = -1;
    screen_width = -1;
    screen_height = -1;
    renderer = nullptr;
    board = nullptr;
    ball_timer_start = -1;
}

Game::Game(SDL_Renderer* in_r, int s_w, int s_h) {
    renderer = in_r;
    screen_width = s_w;
    screen_height = s_h;
    board = new Board();
    board->set_renderer(renderer);
    board->set_screen_size(screen_width, screen_height);
    won_or_lost = 1;

    set_up_sounds();
    set_up_sprites();
    set_up_qbert();
    set_up_enemies();
    set_up_platforms();

    // the plus one is for the snake when it's in ball form
    player->set_enemies(enemies, NUM_ENEMIES);
    player->set_coll_sound(sounds[COLLIDE_SOUND_ID]);
    player->set_fall_sound(sounds[FALL_SOUND_ID]);
    player->set_platforms(platforms, NUM_PLATFORMS);

    snake_timer_start = SDL_GetTicks();
    ball_timer_start = SDL_GetTicks();
}

Game::~Game() {
    delete [] platforms;
    delete [] sounds;
    delete [] enemies;
}

void Game::set_up_sprites() {
    sprites = new SpriteSheet*[NUM_SPRITES];
    for(int i = 0; i < NUM_SPRITES; i++) {
        sprites[i] = new SpriteSheet;
    }
    sprites[QBERT_SPRITE_ID]->set_up(SPRITE_SHEET, 
                                    renderer, 
                                    QBERT_X_SPRITE_POS,
                                    QBERT_Y_SPRITE_POS,
                                    QBERT_SPRITE_WIDTH,
                                    QBERT_SPRITE_HEIGHT,
                                    QBERT_SPRITE_FRAMES,
                                    QBERT_SPRITE_SWITCH_TIME,
                                    true);
    sprites[QBERT_SPRITE_ID]->set_offsets(   QBERT_X_SPRITE_POS, QBERT_Y_SPRITE_POS,
                                            QBERT_X_SPRITE_POS + (QBERT_SPRITE_FRAMES) * QBERT_SPRITE_WIDTH, QBERT_Y_SPRITE_POS,
                                            QBERT_X_SPRITE_POS, QBERT_Y_SPRITE_POS + QBERT_SPRITE_HEIGHT,
                                            QBERT_X_SPRITE_POS + (QBERT_SPRITE_FRAMES) * QBERT_SPRITE_WIDTH, QBERT_Y_SPRITE_POS + QBERT_SPRITE_HEIGHT);
    sprites[RED_BALL_SPRITE_ID]->set_up( SPRITE_SHEET,
                                        renderer,
                                        RED_BALL_X_SPRITE_POS,
                                        RED_BALL_Y_SPRITE_POS,
                                        RED_BALL_SPRITE_WIDTH,
                                        RED_BALL_SPRITE_HEIGHT,
                                        RED_BALL_SPRITE_FRAMES,
                                        RED_BALL_SPRITE_SWITCH_TIME,
                                        false);
    sprites[SNAKE_BALL_SPRITE_ID]->set_up(   SPRITE_SHEET,
                                            renderer,
                                            SNAKE_BALL_X_SPRITE_POS,
                                            SNAKE_BALL_Y_SPRITE_POS,
                                            SNAKE_BALL_SPRITE_WIDTH,
                                            SNAKE_BALL_SPRITE_HEIGHT,
                                            SNAKE_BALL_SPRITE_FRAMES,
                                            SNAKE_BALL_SPRITE_SWITCH_TIME,
                                            false);
    sprites[SNAKE_SPRITE_ID]->set_up(   SPRITE_SHEET,
                                        renderer,
                                        SNAKE_X_SPRITE_POS,
                                        SNAKE_Y_SPRITE_POS,
                                        SNAKE_SPRITE_WIDTH,
                                        SNAKE_SPRITE_HEIGHT,
                                        SNAKE_SPRITE_FRAMES,
                                        SNAKE_SPRITE_SWITCH_TIME,
                                        true);
    sprites[SNAKE_SPRITE_ID]->set_offsets(  SNAKE_X_SPRITE_POS, SNAKE_Y_SPRITE_POS + SNAKE_BALL_SPRITE_HEIGHT + SNAKE_SPRITE_SECOND_LAYER_OFFSET,
                                            SNAKE_X_SPRITE_POS + SNAKE_SPRITE_FRAMES * SNAKE_SPRITE_WIDTH, SNAKE_Y_SPRITE_POS + SNAKE_BALL_SPRITE_HEIGHT + SNAKE_SPRITE_SECOND_LAYER_OFFSET,
                                            SNAKE_X_SPRITE_POS, SNAKE_Y_SPRITE_POS,
                                            SNAKE_X_SPRITE_POS + SNAKE_SPRITE_FRAMES * SNAKE_SPRITE_WIDTH, SNAKE_Y_SPRITE_POS);
    sprites[PLAT_SPRITE_ID]->set_up(SPRITE_SHEET,
                                    renderer,
                                    PLAT_X_SPRITE_POS,
                                    PLAT_Y_SPRITE_POS,
                                    PLAT_SPRITE_WIDTH,
                                    PLAT_SPRITE_HEIGHT,
                                    PLAT_SPRITE_FRAMES,
                                    PLAT_SPRITE_SWITCH_TIME,
                                    false);
}

void Game::set_up_enemies() {
    enemies = new Creature*[NUM_ENEMIES];

    Ball* redball = new Ball(board, sprites[RED_BALL_SPRITE_ID]);
    redball->set_renderer(renderer);
    redball->set_jump_sound(sounds[BALL_JUMP_SOUND_ID]);

    Snake* snake = new Snake(board, sprites[SNAKE_BALL_SPRITE_ID]);
    snake->set_renderer(renderer);
    snake->set_player(player);
    snake->set_snake_sprite(sprites[SNAKE_SPRITE_ID]);
    snake->set_jump_sound(sounds[SNAKE_JUMP_SOUND_ID]);

    enemies[0] = redball;
    enemies[1] = snake;
    enemies[2] = snake->get_ball();
}

void Game::set_up_qbert() {
    player = new Player(board, sprites[QBERT_SPRITE_ID]);
    player->set_renderer(renderer);
    player->set_jump_sound(sounds[QBERT_JUMP_SOUND_ID]);
}

void Game::set_up_platforms() {
    platforms = new Platform*[NUM_PLATFORMS];
    Platform* plat1 = new Platform(board, sprites[PLAT_SPRITE_ID]);
    Platform* plat2 = new Platform(board, sprites[PLAT_SPRITE_ID]);
    platforms[0] = plat1;
    platforms[1] = plat2;
}

void Game::set_up_sounds() {
    sounds = new Sound*[NUM_SOUNDS];
    sounds[QBERT_JUMP_SOUND_ID] = new Sound(QBERT_JUMP);
    sounds[BALL_JUMP_SOUND_ID] = new Sound(BALL_JUMP);
    sounds[COLLIDE_SOUND_ID] = new Sound(COLLIDE);
    sounds[FALL_SOUND_ID] = new Sound(FALL_LOSE);
    sounds[SNAKE_JUMP_SOUND_ID] = new Sound(SNAKE_JUMP);
    sounds[PRE_SPAWN_SOUND_ID] = new Sound(PRE_SPAWN);
    sounds[WIN_SOUND_ID] = new Sound(WIN);
}

bool Game::going() {
    return game_going;
}

void Game::update_snake_timer() {
    if(!enemies[SNAKE_ENEMY_ID]->get_spawned()) {
        if(SDL_GetTicks() - snake_timer_start >= SNAKE_WAIT) {
            enemies[SNAKE_ENEMY_ID]->spawn();
            snake_spawn_sound_played = false;
        } else if (SDL_GetTicks() - snake_timer_start >= SNAKE_WAIT - MINI_TIMER_WAIT && !snake_spawn_sound_played) {
            sounds[PRE_SPAWN_SOUND_ID]->play();
            snake_spawn_sound_played = true;
        }
    } else {
        snake_timer_start = SDL_GetTicks();
        snake_spawn_sound_played = false;
    }
}

void Game::update_ball_timer() {
    if(!enemies[BALL_ENEMY_ID]->get_spawned()) {
        if(SDL_GetTicks() - ball_timer_start >= BALL_WAIT) {
            enemies[BALL_ENEMY_ID]->spawn();
            ball_spawn_sound_played = false;
        } else if (SDL_GetTicks() - ball_timer_start >= BALL_WAIT - MINI_TIMER_WAIT && !ball_spawn_sound_played) {
            sounds[PRE_SPAWN_SOUND_ID]->play();
            ball_spawn_sound_played = true;
        }
    } else {
        ball_timer_start = SDL_GetTicks();
        ball_spawn_sound_played = false;
    }
}

void Game::check_won_lost() {
    if(won_or_lost == 1) {
        if(player->get_alive() == false) {
            won_or_lost = 0;
        }
        if(board->board_filled() == true) {
            won_or_lost = 2;
            sounds[WIN_SOUND_ID]->play();
        }
    }
}

void Game::loop(SDL_Event* event) {

        // draw background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        update_ball_timer();
        update_snake_timer();

        for(int i = 0; i < NUM_PLATFORMS; i++) {    platforms[i]->animate();    }

        if(!player->on_board()) {   
            player->animate(won_or_lost == 1);  
        }
        
        for(int i = 0; i < NUM_ENEMIES - 1; i++) {
            if(!enemies[i]->on_board()) {  
                enemies[i]->animate(won_or_lost == 1);
            }
        }
        
        board->animate();
        if(player->on_board()) { 
            player->animate(won_or_lost == 1);  
        }
        
        for(int i = 0; i < NUM_ENEMIES - 1; i++) {
            if(enemies[i]->on_board()) {   
                enemies[i]->animate(won_or_lost == 1);
            }
        }
        

        SDL_RenderPresent(renderer);

    check_won_lost();
}

void Game::handle_key_press(SDL_Event* event) {
    switch (event->type) {
        case SDL_QUIT:
            game_going = false;
            break;
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym) {
            case SDLK_ESCAPE:
                game_going = false;
                break;
            case SDLK_w:
                player->set_sprite_dir(SpriteSheet::TR);
                player->move(0,-1);
                break;
            case SDLK_a:
                player->set_sprite_dir(SpriteSheet::TL);
                player->move(-1,0);
                break;
            case SDLK_s:
                player->set_sprite_dir(SpriteSheet::BL);
                player->move(0,1);
                break;
            case SDLK_d:
                player->set_sprite_dir(SpriteSheet::BR);
                player->move(1,0);
                break;
            }
            break;
    }
}