#include "threaded_fun.h"

#include <curses.h>
#include <stdlib.h>

#include "data.h"

#define SECOND_TO_NANO(x) (uint64_t)(1000000000.0 * x)
#define EXIT_KEY 3
#define UPDATES_PER_SECOND 3

static void copy_is_playing(mutex_t* mutex, bool playing_src, bool* playing_dst) {
    mutex_lock(mutex);
    *playing_dst = playing_src;
    mutex_unlock(mutex);
}

int32_t gen_rand_range(const int32_t min, const int32_t max, MTRand* seed) {
    const uint32_t difference = max - min;
    uint32_t test;
    do {
        test = genRandLong(seed);
    } while (test >= UINT32_MAX - (UINT32_MAX % difference));
    return test % difference + min;
}

thread_ret timer(void* ns_pointer) {
    const uint64_t target_ns = *(uint64_t*)ns_pointer;
    struct timespec begin_time, end_time;
    timespec_get(&begin_time, TIME_UTC);
    uint64_t elapsed_time;
    do {
        timespec_get(&end_time, TIME_UTC);
        elapsed_time = (end_time.tv_sec - begin_time.tv_sec) * 1000000000 + (end_time.tv_nsec - begin_time.tv_nsec);
    } while (elapsed_time < target_ns);
    return NULL;
}

thread_ret initialize_windows(void* args) {
    window_data_t* window_list = args;
    initscr();
    raw();
    noecho();
    curs_set(0);
    WINDOW* game_window = newwin(BOARD_HEIGHT + 2, BOARD_WIDTH * 2 + 2, 0, 0);
    WINDOW* snake_window = derwin(game_window, BOARD_HEIGHT, BOARD_WIDTH * 2, 1, 1);
    nodelay(snake_window, TRUE);
    box(game_window, 0, 0);
    wrefresh(game_window);
    keypad(snake_window, TRUE);
    window_list->game_window = game_window;
    window_list->snake_window = snake_window;
    return NULL;
}

thread_ret initialize_game(void* args) {
    gameplay_data_t* gameplay = args;
    gameplay->is_playing = true;
    snake_t* snake = &gameplay->snake;
    coordinate_t (*board)[BOARD_HEIGHT][BOARD_WIDTH] = &gameplay->board;
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            (*board)[row][col].x = col;
            (*board)[row][col].y = row;
            (*board)[row][col].is_snake = false;
            (*board)[row][col].is_fruit = false;
        }
    }
    srand(time(NULL));
    gameplay->seed = seedRand((uint32_t)rand());
    snake->length = 3;
    snake->direction = RIGHT;
    snake->body[0] = &(*board)[0][0];
    snake->body[1] = &(*board)[0][1];
    snake->body[2] = &(*board)[0][2];
    snake->head = &(*board)[0][2];
    for (int i = 0; i < 3; ++i) {
        snake->body[i]->is_snake = true;
    }
    gameplay->current_direction = snake->direction;
    while (true) {
        const int rand_x = gen_rand_range(0, BOARD_WIDTH, &gameplay->seed);
        const int rand_y = gen_rand_range(0, BOARD_HEIGHT, &gameplay->seed);
        if (!(*board)[rand_y][rand_x].is_snake) {
            gameplay->current_fruit = &(*board)[rand_y][rand_x];
            gameplay->current_fruit->is_fruit = true;
            break;
        }
    }
    return NULL;
}

thread_ret input_loop(void* args) {
    const data_t* data = args;
    WINDOW* window = data->window->snake_window;
    const mutex_list_t* mutex_list = data->mutex_list;
    bool is_playing = true;
    while (true) {
        copy_is_playing(mutex_list->playing_mutex, data->gameplay->is_playing, &is_playing);
        if (!is_playing) {
            return NULL;
        }
        mutex_lock(mutex_list->ui_mutex);
        const int key = wgetch(window);
        if (key != ERR && key != KEY_MOUSE && key != KEY_RESIZE && key != KEY_EVENT) {
            if (key == EXIT_KEY) {
                wrefresh(window);
                mutex_unlock(mutex_list->ui_mutex);
                copy_is_playing(mutex_list->playing_mutex, false, &data->gameplay->is_playing);
                return NULL;
            }
            if (key == KEY_LEFT) {
                mutex_lock(mutex_list->gameplay_mutex);
                data->gameplay->current_direction = LEFT;
                mutex_unlock(mutex_list->gameplay_mutex);
            } else if (key == KEY_RIGHT) {
                mutex_lock(mutex_list->gameplay_mutex);
                data->gameplay->current_direction = RIGHT;
                mutex_unlock(mutex_list->gameplay_mutex);
            } else if (key == KEY_UP) {
                mutex_lock(mutex_list->gameplay_mutex);
                data->gameplay->current_direction = UP;
                mutex_unlock(mutex_list->gameplay_mutex);
            } else if (key == KEY_DOWN) {
                mutex_lock(mutex_list->gameplay_mutex);
                data->gameplay->current_direction = DOWN;
                mutex_unlock(mutex_list->gameplay_mutex);
            }
        }
        wrefresh(window);
        mutex_unlock(mutex_list->ui_mutex);
    }
}

thread_ret update_ui(void* args) {
    const data_t* data = args;
    const mutex_list_t* mutex_list = data->mutex_list;
    const gameplay_data_t* gameplay_data = data->gameplay;
    const window_data_t* window_data = data->window;
    bool is_playing = true;
    while (true) {
        copy_is_playing(mutex_list->playing_mutex, gameplay_data->is_playing, &is_playing);
        if (!is_playing) {
            return NULL;
        }
        mutex_lock(mutex_list->ui_mutex);
        werase(window_data->snake_window);
        mutex_lock(mutex_list->gameplay_mutex);
        for (int i = 0; i < gameplay_data->snake.length; ++i) {
            mvwaddch(window_data->snake_window, gameplay_data->snake.body[i]->y, gameplay_data->snake.body[i]->x * 2,
                     'X');
        }
        mvwaddch(window_data->snake_window, gameplay_data->current_fruit->y, gameplay_data->current_fruit->x * 2, 'A');
        mutex_unlock(mutex_list->gameplay_mutex);
        wrefresh(window_data->snake_window);
        mutex_unlock(mutex_list->ui_mutex);
    }
}

thread_ret gameplay_loop(void* args) {
    const data_t* data = args;
    thread_t timer_thread;
    mutex_t* game_mutex = data->mutex_list->gameplay_mutex;
    gameplay_data_t* gameplay_data = data->gameplay;
    uint64_t target_time = SECOND_TO_NANO(1.0 / UPDATES_PER_SECOND);
    bool is_playing = true;
    while (true) {
        thread_create(&timer_thread, timer, &target_time);
        copy_is_playing(data->mutex_list->playing_mutex, gameplay_data->is_playing, &is_playing);
        if (!is_playing) {
            thread_join(timer_thread, NULL);
            return NULL;
        }
        mutex_lock(game_mutex);
        const status_t status = update_snake(&gameplay_data->snake, gameplay_data->current_direction,
                                             &gameplay_data->board);
        if ((status & ATE_FRUIT) == ATE_FRUIT) {
            while (true) {
                const int rand_x = gen_rand_range(0, BOARD_WIDTH, &gameplay_data->seed);
                const int rand_y = gen_rand_range(0, BOARD_HEIGHT, &gameplay_data->seed);
                if (!gameplay_data->board[rand_y][rand_x].is_snake) {
                    gameplay_data->current_fruit->is_fruit = false;
                    gameplay_data->current_fruit = &gameplay_data->board[rand_y][rand_x];
                    gameplay_data->current_fruit->is_fruit = true;
                    break;
                }
            }
        } else if ((status & HIT_SELF) == HIT_SELF) {
            mutex_unlock(game_mutex);
            copy_is_playing(data->mutex_list->playing_mutex, false, &gameplay_data->is_playing);
            thread_join(timer_thread, NULL);
            return NULL;
        }
        mutex_unlock(game_mutex);
        thread_join(timer_thread, NULL);
    }
}
