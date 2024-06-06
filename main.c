#include <stdio.h>
#include <stdlib.h>
#include "snake.h"

void print_board(coordinate_t board[][BOARD_WIDTH]);

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
    snake_t snake;
    for (int i = 0; i < BOARD_HEIGHT * BOARD_WIDTH; i++) {
        snake.body[i] = NULL;
    }
    snake.length = 3;
    snake.direction = right;
    for (int i = 0; i < 3; i++) {
        snake.body[i] = &board[0][i];
        snake.body[i]->is_snake = true;
    }
    snake.head = snake.body[snake.length - 1];
    print_board(board);
    update_snake(&snake, down, board);
    print_board(board);
    update_snake(&snake, down, board);
    print_board(board);
    return 0;
}

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
                output[index] = 'O';
            }
            index++;
            output[index] = ' ';
            index++;
        }
        output[index] = '\n';
        index++;
    }
    output[index] = '\0';
    printf("%s\n", output);
}
