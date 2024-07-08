#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "snake.h"
#include "lib/mtwister.h"

#define MILLI_TO_NANO(x) (uint64_t)(1000000.0 * x)
#define SECOND_TO_NANO(x) (uint64_t)(1000000000.0 * x)
#define ENTER_KEY 10
#define EXIT_KEY 2

void print_board(coordinate_t board[][BOARD_WIDTH]);

int gen_rand_range(int min, int max, MTRand* seed);

void* input_loop(void* win_pointer);

void* initialize_windows(void* args);

void* initialize_game(void* args);

void* timer(void* ns_pointer);

void* timer_window_func(void* win_pointer);

static char current_key = ERR;

/*key_code nb_getkey() {
    if (kbhit()) {
        return getkey();
    }
    return 0;
}*/

typedef struct window {
    WINDOW* game_window;
    WINDOW* snake_window;
} window_data_t;

typedef struct mutex {
    pthread_mutex_t* ui_mutex;
    pthread_mutex_t* gameplay_mutex;
} mutex_list_t;

typedef struct full {
    gameplay_data_t* gameplay;
    window_data_t* window;
    mutex_list_t* mutex_list;
} data_t;

void lock_mutex(pthread_mutex_t* mutex) {
    while (pthread_mutex_trylock(mutex) != 0) { }
}

void unlock_mutex(pthread_mutex_t* mutex) {
    pthread_mutex_unlock(mutex);
}

int main() {
    pthread_t input_thread, window_init_thread, gameplay_init_thread, ui_thread;
    pthread_mutex_t ui_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t gameplay_mutex = PTHREAD_MUTEX_INITIALIZER;
    mutex_list_t list = {
            .ui_mutex = &ui_mutex,
            .gameplay_mutex = &gameplay_mutex
    };
    pthread_mutex_init(&ui_mutex, NULL);
    pthread_mutex_init(&gameplay_mutex, NULL);
    window_data_t window_data;
    gameplay_data_t gameplay_data;
    pthread_create(&window_init_thread, NULL, initialize_windows, &window_data);
    coordinate_t board[BOARD_HEIGHT][BOARD_WIDTH];
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            board[row][col].x = col;
            board[row][col].y = row;
            board[row][col].is_snake = false;
            board[row][col].is_fruit = false;
        }
    }
    coordinate_t* current_fruit;

    snake_t snake = {
            .length = 3,
            .direction = RIGHT,
            .body = {&board[0][0], &board[0][1], &board[0][2]},
            .head = &board[0][2]
    };
    for (int i = 0; i < 3; ++i) {
        snake.body[i]->is_snake = true;
    }
    direction_t direction = snake.direction;
    while (true) {
        const int rand_x = gen_rand_range(0, BOARD_WIDTH, &seed);
        const int rand_y = gen_rand_range(0, BOARD_HEIGHT, &seed);
        if (!board[rand_y][rand_x].is_snake) {
            current_fruit = &board[rand_y][rand_x];
            current_fruit->is_fruit = true;
            break;
        }
    }
    void* window_return;
    pthread_join(window_init_thread, &window_return);
    WINDOW* game_window = window_data.game_window;
    WINDOW* snake_window = window_data.snake_window;
    pthread_create(&input_thread, NULL, input_loop, snake_window);
    pthread_join(input_thread, NULL);
    delwin(snake_window);
    delwin(game_window);
    endwin();
    return 0;
}

/**
 * 
 * @param ns_pointer A pointer to an uint64_t specifying the amount of nanoseconds that should pass before returning
 * @return Returns nothing
 */
void* timer(void* ns_pointer) {
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

void* initialize_windows(void* args) {
    window_data_t* window = (window_data_t*) args;
    initscr();
    raw();
    noecho();
    WINDOW* game_window = newwin(BOARD_HEIGHT + 2, BOARD_WIDTH * 2 + 2, 0, 0);
    WINDOW* snake_window = derwin(game_window, BOARD_HEIGHT, BOARD_WIDTH * 2, 1, 1);
    nodelay(snake_window, TRUE);
    box(game_window, 0, 0);
    wrefresh(game_window);
    keypad(snake_window, TRUE);
    window->game_window = game_window;
    window->snake_window = snake_window;
    return (void*) window;
}

void* initialize_game(void* args) {
    gameplay_data_t* gameplay = (gameplay_data_t*) args;
    snake_t* snake = &gameplay->snake;
    coordinate_t (* board)[BOARD_HEIGHT][BOARD_WIDTH] = &gameplay->board;
    for (int row = 0; row < BOARD_HEIGHT; ++row) {
        for (int col = 0; col < BOARD_WIDTH; ++col) {
            board[row][col]->x = col;
            board[row][col]->y = row;
            board[row][col]->is_snake = false;
            board[row][col]->is_fruit = false;
        }
    }
    srand(time(NULL));
    gameplay->seed = seedRand((uint32_t) rand());
    snake->length = 3;
    snake->direction = RIGHT;
    snake->body[0] = board[0][0];
    snake->body[1] = board[0][1];
    snake->body[2] = board[0][2];
    snake->head = board[0][2];
    for (int i = 0; i < 3; ++i) {
        snake->body[i]->is_snake = true;
    }
    gameplay->current_direction = snake->direction;
    while (true) {
        const int rand_x = gen_rand_range(0, BOARD_WIDTH, &gameplay->seed);
        const int rand_y = gen_rand_range(0, BOARD_HEIGHT, &gameplay->seed);
        if (!board[rand_y][rand_x]->is_snake) {
            gameplay->current_fruit = board[rand_y][rand_x];
            gameplay->current_fruit->is_fruit = true;
            break;
        }
    }
    return NULL;
}

/**
 * @param win_pointer A pointer to the WINDOW used for input control
 * @return Returns nothing
 */
void* input_loop(void* win_pointer) {
    WINDOW* window = (WINDOW*) win_pointer;
    while (true) {
        int key = wgetch(window);
        if (key != ERR && key != KEY_MOUSE && key != KEY_RESIZE && key != KEY_EVENT) {
            if (key == EXIT_KEY) {
                return NULL;
            } else if (key == KEY_LEFT) {
                wprintw(window, "Left");
            } else if (key == KEY_RIGHT) {
                wprintw(window, "Right");
            } else if (key == KEY_DOWN) {
                wprintw(window, "Down");
            } else if (key == KEY_UP) {
                wprintw(window, "Up");
            } else if (key == ENTER_KEY) {
                wprintw(window, "Enter");
            } else {
                waddch(window, key);
            }
        }
        wrefresh(window);
    }
}

void* update_ui(void* args) {

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



void print_board(coordinate_t board[][BOARD_WIDTH]) {
    char output[(2 * BOARD_WIDTH + 1) * BOARD_HEIGHT + 1];
    int index = 0;
    for (int row = 0; row < BOARD_HEIGHT; row++) {
        for (int col = 0; col < BOARD_WIDTH; col++) {
            if (board[row][col].is_snake) {
                output[index] = 'X';
            } else if (board[row][col].is_fruit) {
                output[index] = 'A';
            } else {
                output[index] = ' ';
            }
            index++;
            output[index] = ' ';
            index++;
        }
        output[index] = '\n';
        index++;
    }
    output[index] = '\0';
    printf("%s", output);
}

int gen_rand_range(const int min, const int max, MTRand* seed) {
    const int difference = max - min;
    uint32_t test;
    do {
        test = genRandLong(seed);
    } while (test >= UINT32_MAX - UINT32_MAX % difference);
    return test % difference + min;
}


