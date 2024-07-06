cmake_minimum_required(VERSION 3.29)
project(Snake C)

set(CMAKE_C_STANDARD 17)

if (WIN32 OR MSVC)
    add_subdirectory(lib/pdcurses)
else ()
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIRS})
endif ()

add_executable(Snake main.c
        snake.h
        snake.c
        lib/rogueutil.h
        lib/mtwister.c)

if (WIN32 OR MSVC)
    target_link_libraries(Snake PRIVATE PDCurses)
else ()
    target_link_libraries(Snake ${CURSES_LIBRARIES})
endif ()