#ifndef SNAKE_DATA_H
#define SNAKE_DATA_H

#include <curses.h>

#include "snake.h"
#include "thrdef.h"
#include "../lib/mtwister/mtwister.h"

typedef struct gameplay_data {
    MTRand seed;
    snake_t snake;
    coordinate_t board[BOARD_HEIGHT][BOARD_WIDTH];
    coordinate_t* current_fruit;
    direction_t current_direction;
    bool is_playing;
} gameplay_data_t;

typedef struct window_data {
    WINDOW* game_window;
    WINDOW* snake_window;
} window_data_t;

typedef struct mutex_list {
    mutex_t* ui_mutex;
    mutex_t* gameplay_mutex;
    mutex_t* playing_mutex;
} mutex_list_t;

typedef struct full_data {
    gameplay_data_t* gameplay;
    window_data_t* window;
    mutex_list_t* mutex_list;
} data_t;
#endif //SNAKE_DATA_H
