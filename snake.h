#ifndef SNAKE_SNAKE_H
#define SNAKE_SNAKE_H

#include <stdint.h>
#include <stdbool.h>

#define BOARD_WIDTH 17
#define BOARD_HEIGHT 15

typedef enum direction {
    up,
    down,
    left,
    right
} direction_t;

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

void update_snake(snake_t* snake, direction_t direction, coordinate_t board[][BOARD_WIDTH]);

void update_direction(snake_t* snake, direction_t direction);


#endif //SNAKE_SNAKE_H
