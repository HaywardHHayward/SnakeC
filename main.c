#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "snake.h"
#include "thrdef.h"
#include "lib/mtwister.h"

#define SECOND_TO_NANO(x) (uint64_t)(1000000000.0 * x)
#define EXIT_KEY 2
#define UPDATES_PER_SECOND 3

int32_t gen_rand_range(int32_t min, int32_t max, MTRand* seed);

thread_ret input_loop(void* args);

thread_ret initialize_windows(void* args);

thread_ret initialize_game(void* args);

thread_ret timer(void* ns_pointer);

thread_ret gameplay_loop(void* args);

thread_ret update_ui(void* args);

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

int main() {
    thread_t input_thread, ui_thread, gameplay_thread;
    mutex_t ui_mutex, gameplay_mutex;
    mutex_list_t mutex_list = {
            .ui_mutex = &ui_mutex,
            .gameplay_mutex = &gameplay_mutex
    };
    mutex_init(&ui_mutex);
    mutex_init(&gameplay_mutex);
    window_data_t window_data;
    gameplay_data_t gameplay_data;
    thread_create(&ui_thread, initialize_windows, &window_data);
    thread_create(&gameplay_thread, initialize_game, &gameplay_data);
    thread_join(ui_thread, NULL);
    WINDOW* game_window = window_data.game_window;
    WINDOW* snake_window = window_data.snake_window;
    data_t data = {
            .window = &window_data,
            .gameplay = &gameplay_data,
            .mutex_list = &mutex_list
    };
    thread_join(gameplay_thread, NULL);
    thread_create(&input_thread, input_loop, &data);
    thread_create(&gameplay_thread, gameplay_loop, &data);
    thread_create(&ui_thread, update_ui, &data);
    thread_detach(gameplay_thread);
    thread_detach(ui_thread);
    thread_join(input_thread, NULL);
    delwin(snake_window);
    delwin(game_window);
    endwin();
    return 0;
}

thread_ret timer(void* ns_pointer) {
    uint64_t target_ns = *(uint64_t*) ns_pointer;
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
    window_data_t* window = (window_data_t*) args;
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
    window->game_window = game_window;
    window->snake_window = snake_window;
    return NULL;
}

thread_ret initialize_game(void* args) {
    gameplay_data_t* gameplay = (gameplay_data_t*) args;
    snake_t* snake = &gameplay->snake;
    coordinate_t (* board)[BOARD_HEIGHT][BOARD_WIDTH] = &gameplay->board;
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            (*board)[row][col].x = col;
            (*board)[row][col].y = row;
            (*board)[row][col].is_snake = false;
            (*board)[row][col].is_fruit = false;
        }
    }
    srand(time(NULL));
    gameplay->seed = seedRand((uint32_t) rand());
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
    data_t* data = (data_t*) args;
    WINDOW* window = data->window->snake_window;
    mutex_list_t* mutex_list = data->mutex_list;
    while (true) {
        mutex_lock(mutex_list->ui_mutex);
        int key = wgetch(window);
        if (key != ERR && key != KEY_MOUSE && key != KEY_RESIZE && key != KEY_EVENT) {
            if (key == EXIT_KEY) {
                wrefresh(window);
                mutex_unlock(mutex_list->ui_mutex);
                return NULL;
            } else if (key == KEY_LEFT) {
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
    data_t* data = (data_t*) args;
    mutex_list_t* mutex_list = data->mutex_list;
    gameplay_data_t* gameplay_data = data->gameplay;
    window_data_t* window_data = data->window;
    while (true) {
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
    data_t* data = (data_t*) args;
    thread_t timer_thread;
    mutex_t* mutex = data->mutex_list->gameplay_mutex;
    gameplay_data_t* gameplay_data = data->gameplay;
    uint64_t target_time = SECOND_TO_NANO(1.0 / UPDATES_PER_SECOND);
    while (true) {
        thread_create(&timer_thread, timer, &target_time);
        mutex_lock(mutex);
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
            mutex_unlock(mutex);
            thread_join(timer_thread, NULL);
            break;
        }
        mutex_unlock(mutex);
        thread_join(timer_thread, NULL);
    }
    return NULL;
}

/*int main(void) {
    hidecursor();
    srand(time(NULL));
    MTRand seed = seedRand((uint32_t)rand());
    coordinate_t board[BOARD_HEIGHT][BOARD_WIDTH];
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j].y = i;
            board[i][j].x = j;
            board[i][j].is_snake = false;
            board[i][j].is_fruit = false;
        }
    }
    coordinate_t* current_fruit;
    while (true) {
        const int rand_x = gen_rand_range(0, BOARD_WIDTH, &seed);
        const int rand_y = gen_rand_range(0, BOARD_HEIGHT, &seed);
        if (!board[rand_y][rand_x].is_snake) {
            current_fruit = &board[rand_y][rand_x];
            current_fruit->is_fruit = true;
            break;
        }
    }
    snake_t snake = {
        .length = 3,
        .direction = RIGHT,
        .body = {&board[0][0], &board[0][1], &board[0][2]},
        .head = &board[0][2]
    };
    for (int i = 0; i < 3; i++) {
        snake.body[i]->is_snake = true;
    }
    direction_t direction = snake.direction;
    while (true) {
        int key_pressed = 0;
        for (int frame = 0; frame < 15; frame++) {
            const int buffer_pressed = nb_getkey();
            if (buffer_pressed == KEY_UP || buffer_pressed == KEY_DOWN || buffer_pressed == KEY_LEFT || buffer_pressed
                == KEY_RIGHT) {
                key_pressed = buffer_pressed;
            }
            const int frame_delay = frame % 3 == 0 ? 16 : 17;
            // this essentially makes frame_delay 16.666 repeating, making it about 60fps
            msleep(frame_delay);
        }
        switch (key_pressed) {
            case KEY_UP:
                direction = UP;
                break;
            case KEY_DOWN:
                direction = DOWN;
                break;
            case KEY_LEFT:
                direction = LEFT;
                break;
            case KEY_RIGHT:
                direction = RIGHT;
                break;
            default:
                break;
        }
        const status_t status = update_snake(&snake, direction, board);
        print_board(board);
        if ((status & ATE_FRUIT) == ATE_FRUIT) {
            while (true) {
                const int rand_x = gen_rand_range(0, BOARD_WIDTH, &seed);
                const int rand_y = gen_rand_range(0, BOARD_HEIGHT, &seed);
                if (!board[rand_y][rand_x].is_snake) {
                    current_fruit->is_fruit = false;
                    current_fruit = &board[rand_y][rand_x];
                    current_fruit->is_fruit = true;
                    break;
                }
            }
        } else if ((status & HIT_SELF) == HIT_SELF) {
            break;
        }
    }
    printf("Press enter to exit...");
    getchar();
    showcursor();
    return 0;
}*/

int32_t gen_rand_range(const int32_t min, const int32_t max, MTRand* seed) {
    const uint32_t difference = max - min;
    uint32_t test;
    do {
        test = genRandLong(seed);
    } while (test >= UINT32_MAX - (UINT32_MAX % difference));
    return (int32_t) ((test % difference) + min);
}


