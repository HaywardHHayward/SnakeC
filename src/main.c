#include <curses.h>

#include "data.h"
#include "snake.h"
#include "thrdef.h"
#include "threaded_fun.h"

int main() {
    window_data_t window_data;
    gameplay_data_t gameplay_data;
    thread_t input_thread, ui_thread, gameplay_thread;
    thread_create(&ui_thread, initialize_windows, &window_data);
    thread_create(&gameplay_thread, initialize_game, &gameplay_data);
    mutex_t ui_mutex, gameplay_mutex, condition_mutex;
    mutex_init(&ui_mutex);
    mutex_init(&gameplay_mutex);
    mutex_init(&condition_mutex);
    mutex_list_t mutex_list = {
        .ui_mutex = &ui_mutex,
        .gameplay_mutex = &gameplay_mutex,
        .playing_mutex = &condition_mutex
    };
    thread_join(ui_thread, NULL);
    WINDOW* game_window = window_data.game_window;
    WINDOW* snake_window = window_data.snake_window;
    thread_join(gameplay_thread, NULL);
    data_t data = {.window = &window_data, .gameplay = &gameplay_data, .mutex_list = &mutex_list,};
    thread_join(gameplay_thread, NULL);
    thread_create(&input_thread, input_loop, &data);
    thread_create(&gameplay_thread, gameplay_loop, &data);
    thread_create(&ui_thread, update_ui, &data);
    thread_join(input_thread, NULL);
    thread_join(ui_thread, NULL);
    thread_join(gameplay_thread, NULL);
    delwin(snake_window);
    delwin(game_window);
    endwin();
    return 0;
}
