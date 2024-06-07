cmake_minimum_required(VERSION 3.28)
project(Snake C)

set(CMAKE_C_STANDARD 17)

add_executable(Snake main.c
        snake.h
        snake.c
        rogueutil.h)
