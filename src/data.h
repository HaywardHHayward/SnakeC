#ifndef SNAKE_DATA_H
#define SNAKE_DATA_H

#include "thrdef.h"
#include "snake.h"
#include <time.h>
#include <curses.h>

typedef struct window {
    WINDOW* game_window;
    WINDOW* snake_window;
} window_data_t;
typedef struct mutex {
    mutex_t* ui_mutex;
    mutex_t* gameplay_mutex;
} mutex_list_t;
typedef struct full {
    gameplay_data_t* gameplay;
    window_data_t* window;
    mutex_list_t* mutex_list;
} data_t;
#endif //SNAKE_DATA_H
