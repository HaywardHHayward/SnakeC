#ifndef SNAKE_SNAKE_H
#define SNAKE_SNAKE_H

#include <stdbool.h>
#include <stdint.h>

#define BOARD_WIDTH 30
#define BOARD_HEIGHT 15
#define INVALID_DIRECTION 3
// The sum of opposite directions will always equal INVALID_DIRECTION

typedef enum direction {
    UP    = 0, //0b00
    DOWN  = 3, //0b11
    LEFT  = 1, //0b01
    RIGHT = 2, //0b10
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
    uint8_t y : 6;
    uint8_t x;
} coordinate_t;

typedef struct snake {
    coordinate_t* body[BOARD_HEIGHT * BOARD_WIDTH];
    coordinate_t* head;
    direction_t direction : 2;
    uint16_t length;
} snake_t;

status_t update_snake(snake_t* snake, direction_t direction, coordinate_t (*board)[BOARD_HEIGHT][BOARD_WIDTH]);

#endif //SNAKE_SNAKE_H
