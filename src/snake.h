#ifndef SNAKE_SNAKE_H
#define SNAKE_SNAKE_H

#include <stdbool.h>
#include <stdint.h>
#include "../lib/mtwister/mtwister.h"

#define BOARD_WIDTH 17
#define BOARD_HEIGHT 15

typedef enum direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction_t;

typedef enum status {
    STANDARD  = 0,
    ATE_FRUIT = 1 << 0,
    HIT_SELF  = 1 << 1,
    WON_GAME  = 1 << 3
} status_t;

typedef struct coordinate {
    bool is_snake : 1;
    bool is_fruit : 1;
    uint8_t x : 5; // 5 bits needed to have a width of 17
    uint8_t y : 4; // 4 bits needed to have a height of 15
} coordinate_t;

typedef struct snake {
    coordinate_t* body[BOARD_HEIGHT * BOARD_WIDTH];
    coordinate_t* head;
    direction_t direction;
    uint16_t length;
} snake_t;

typedef struct gameplay {
    MTRand seed;
    snake_t snake;
    coordinate_t board[BOARD_HEIGHT][BOARD_WIDTH];
    coordinate_t* current_fruit;
    direction_t current_direction;
} gameplay_data_t;



status_t update_snake(snake_t* snake, direction_t direction, coordinate_t (* board)[BOARD_HEIGHT][BOARD_WIDTH]);

void update_direction(snake_t* snake, direction_t direction);

#endif //SNAKE_SNAKE_H
