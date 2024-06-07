#include <stdio.h>
#include <stdlib.h>
#include "snake.h"
#include "rogueutil/rogueutil.h"


void print_board(coordinate_t board[][BOARD_WIDTH]);

key_code nb_getkey() {
    if (kbhit()) {
        return getkey();
    }
    return 0;
}

int main(void) {
    coordinate_t board[BOARD_HEIGHT][BOARD_WIDTH];
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            board[i][j].y = i;
            board[i][j].x = j;
            board[i][j].is_snake = false;
            board[i][j].is_fruit = false;
        }
    }
    board[2][2].is_fruit = true;
    snake_t snake = {.length = 3, .direction = RIGHT, .body = {&board[0][0], &board[0][1],
                                                               &board[0][2]}, .head = &board[0][2]};
    for (int i = 0; i < 3; i++) {
        snake.body[i]->is_snake = true;
    }
    direction_t direction = snake.direction;
    while (1) {
        int key_pressed = 0;
        int frame_delay;
        for (int frame = 0; frame < 21; frame++) {
            int buffer_pressed = nb_getkey();
            if (buffer_pressed == KEY_UP ||
                buffer_pressed == KEY_DOWN ||
                buffer_pressed == KEY_LEFT ||
                buffer_pressed == KEY_RIGHT) {
                key_pressed = buffer_pressed;
            }
            frame_delay = (frame % 3 == 0) ? 16 : 17;
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
        update_snake(&snake, direction, board);
        print_board(board);
    }
    return 0;
}

void print_board(coordinate_t board[][BOARD_WIDTH]) {
    cls();
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
