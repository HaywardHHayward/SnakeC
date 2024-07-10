#include <curses.h>

#include "snake.h"
#include "thrdef.h"
#include "threaded_fun.h"
#include "data.h"

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




