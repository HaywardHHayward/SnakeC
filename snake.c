#include "snake.h"

void update_direction(snake_t* snake, direction_t direction) {
    if (direction != snake->direction) {
        switch (snake->direction) {
            case up:
                if (direction != down) {
                    snake->direction = direction;
                }
                break;
            case down:
                if (direction != up) {
                    snake->direction = direction;
                }
                break;
            case left:
                if (direction != right) {
                    snake->direction = direction;
                }
                break;
            case right:
                if (direction != left) {
                    snake->direction = direction;
                }
                break;
        }
    }
}

void update_snake(snake_t* snake, direction_t direction, coordinate_t board[][BOARD_WIDTH]) {
    update_direction(snake, direction);
    int16_t new_x = snake->head->x;
    int16_t new_y = snake->head->y;
    switch (snake->direction) {
        case up:
            new_y--;
            break;
        case down:
            new_y++;
            break;
        case left:
            new_x--;
            break;
        case right:
            new_x++;
            break;
    }
    if (new_x < 0 || new_x >= BOARD_WIDTH) {
        return;
    }
    if (new_y < 0 || new_y >= BOARD_HEIGHT) {
        return;
    }
    coordinate_t* new_head = &board[new_y][new_x];
    for (int i = 0; i < snake->length - 1; i++) {
        if (i == 0) {
            snake->body[0]->is_snake = false;
        }
        snake->body[i] = snake->body[i + 1];
    }
    snake->body[snake->length - 1] = new_head;
    new_head->is_snake = true;
    snake->head = new_head;
}