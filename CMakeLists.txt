cmake_minimum_required(VERSION 3.29)
project(Snake C)

set(CMAKE_C_STANDARD 17)

if (WIN32 OR MSVC)
    find_package(Curses QUIET)
    if (CURSES_FOUND)
        include_directories(${CURSES_INCLUDE_DIRS})
    else ()
        add_subdirectory(lib/pdcurses)
    endif ()
else ()
    find_package(Curses REQUIRED)
    include_directories(${CURSES_INCLUDE_DIRS})
endif ()

find_package(Threads REQUIRED)

add_subdirectory(lib/mtwister)

add_executable(Snake src/main.c
        src/snake.c
        src/threaded_fun.c
)

if (CURSES_FOUND)
    target_link_libraries(Snake PRIVATE ${CURSES_LIBRARIES})
else ()
    target_link_libraries(Snake PRIVATE PDCurses)
endif ()

target_link_libraries(Snake PRIVATE Threads::Threads)

target_link_libraries(Snake PRIVATE MTwister)