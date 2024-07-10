//
// Created by Hayden Reckward on 7/9/24.
//

#ifndef SNAKE_THREADED_FUN_H
#define SNAKE_THREADED_FUN_H

#include "../lib/mtwister/mtwister.h"
#include "thrdef.h"
#include "snake.h"
#include <time.h>
#include <stdlib.h>
#include <curses.h>
#include "data.h"

int32_t gen_rand_range(int32_t min, int32_t max, MTRand* seed);

void* timer(void* ns_pointer);

void* initialize_windows(void* args);

void* initialize_game(void* args);

void* input_loop(void* args);

void* update_ui(void* args);

void* gameplay_loop(void* args);

#endif //SNAKE_THREADED_FUN_H
