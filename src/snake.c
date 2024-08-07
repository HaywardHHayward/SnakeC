#include "snake.h"

static void update_direction(snake_t* snake, const direction_t direction) {
    if (direction == snake->direction || direction + snake->direction == INVALID_DIRECTION) {
        return;
    }
    // direction_t is defined in such a way that opposite directions always sum to INVALID_DIRECTION
    snake->direction = direction;
}

status_t update_snake(snake_t* snake, const direction_t direction, coordinate_t (*board)[BOARD_HEIGHT][BOARD_WIDTH]) {
    status_t status = STANDARD;
    update_direction(snake, direction);
    int16_t new_x = snake->head->x;
    int16_t new_y = snake->head->y;
    switch (snake->direction) {
        case UP:
            new_y--;
            break;
        case DOWN:
            new_y++;
            break;
        case LEFT:
            new_x--;
            break;
        case RIGHT:
            new_x++;
            break;
    }
    if (new_x < 0) {
        new_x = BOARD_WIDTH - 1;
    } else if (new_x >= BOARD_WIDTH) {
        new_x = 0;
    }
    if (new_y < 0) {
        new_y = BOARD_HEIGHT - 1;
    } else if (new_y >= BOARD_HEIGHT) {
        new_y = 0;
    }
    coordinate_t* new_head = &(*board)[new_y][new_x];
    if (new_head->is_snake) {
        return HIT_SELF;
    }
    if (new_head->is_fruit) {
        snake->length++;
        status |= ATE_FRUIT;
    } else {
        for (int i = 0; i < snake->length - 1; i++) {
            if (i == 0) {
                snake->body[0]->is_snake = false;
            }
            snake->body[i] = snake->body[i + 1];
        }
    }
    snake->body[snake->length - 1] = new_head;
    new_head->is_snake = true;
    snake->head = new_head;
    if (snake->length == BOARD_HEIGHT * BOARD_WIDTH) {
        status |= WON_GAME;
    }
    return status;
}
