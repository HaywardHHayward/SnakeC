#include <curses.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "snake.h"
#include "lib/mtwister.h"


void print_board(coordinate_t board[][BOARD_WIDTH]);

int gen_rand_range(int min, int max, MTRand* seed);

void* input_loop(void* args);

void* initialize_windows(void* args);

static int current_key = ERR;

/*key_code nb_getkey() {
    if (kbhit()) {
        return getkey();
    }
    return 0;
}*/

int main() {
    pthread_t input_thread, window_thread;
    void* window_return = NULL;
    pthread_create(&window_thread, NULL, initialize_windows, NULL);
    pthread_join(window_thread, &window_return);
    WINDOW** window_result = window_return;
    WINDOW* snake_window = window_result[1];
    WINDOW* game_window = window_result[0];
    free(window_result);
    wprintw(snake_window, "Hello world!");
    wrefresh(game_window);
    void* args = game_window;
    pthread_create(&input_thread, NULL, input_loop, args);
    pthread_join(input_thread, NULL);
    delwin(snake_window);
    delwin(game_window);
    endwin();
    return 0;
}

void* initialize_windows(void* args) {
    initscr();
    raw();
    nodelay(stdscr, true);
    noecho();
    WINDOW* game_window = newwin(BOARD_HEIGHT + 2, BOARD_WIDTH * 2 + 2, 0, 0);
    WINDOW* snake_window = derwin(game_window, BOARD_HEIGHT, BOARD_WIDTH * 2, 1, 1);
    box(game_window, 0, 0);
    WINDOW** window = calloc(sizeof (WINDOW*), 2);
    window[0] = game_window;
    window[1] = snake_window;
    void* return_value = (void*)window;
    return return_value;
}

void* input_loop(void* args) {
    WINDOW* window = (WINDOW*)args;
    while(true){
        int key = wgetch(window);
        if (key != ERR && key != KEY_MOUSE) {
            current_key = key;
        }
        if (current_key != ERR){
            pthread_exit(NULL);
        }
    }
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
