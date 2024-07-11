#ifndef SNAKE_THREADED_FUN_H
#define SNAKE_THREADED_FUN_H

#include <mtwister.h>

#include "thread_port.h"

int32_t gen_rand_range(int32_t min, int32_t max, MTRand* seed);

thread_ret timer(void* ns_pointer);

thread_ret initialize_windows(void* args);

thread_ret initialize_game(void* args);

thread_ret input_loop(void* args);

thread_ret update_ui(void* args);

thread_ret gameplay_loop(void* args);

#endif //SNAKE_THREADED_FUN_H
