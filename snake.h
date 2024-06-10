#ifndef SNAKE_SNAKE_H
#define SNAKE_SNAKE_H

#include <stdbool.h>
#include <stdint.h>

#define BOARD_WIDTH 17
#define BOARD_HEIGHT 15

typedef enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction_t;

typedef enum status {
    STANDARD = 0,
    ATE_FRUIT = 1 << 0,
    HIT_SELF = 1 << 1,
    WON_GAME = 1 << 3
} status_t;

typedef struct coordinate {
    bool is_snake;
    bool is_fruit;
    uint8_t x;
    uint8_t y;
} coordinate_t;

typedef struct snake {
    coordinate_t* body[BOARD_HEIGHT * BOARD_WIDTH];
    coordinate_t* head;
    uint8_t length;
    direction_t direction;
} snake_t;

status_t update_snake(snake_t* snake, direction_t direction, coordinate_t board[][17]);

void update_direction(snake_t* snake, direction_t direction);


#endif //SNAKE_SNAKE_H
