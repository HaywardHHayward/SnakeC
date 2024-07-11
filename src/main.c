#include <curses.h>
#include <string.h>

#include "data.h"
#include "threaded_fun.h"
#include "thread_port.h"

thread_ret main_menu(void* args);

int main() {
    initscr();
    raw();
    noecho();
    curs_set(0);
    window_data_t window_data;
    gameplay_data_t gameplay_data;
    thread_t misc_thread, ui_thread, gameplay_thread;
    thread_create(&misc_thread, main_menu, NULL);
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
    data_t data = {.window = &window_data, .gameplay = &gameplay_data, .mutex_list = &mutex_list};
    thread_join(misc_thread, NULL);
    wrefresh(game_window);
    thread_create(&misc_thread, input_loop, &data);
    thread_create(&gameplay_thread, gameplay_loop, &data);
    thread_create(&ui_thread, update_ui, &data);
    thread_join(misc_thread, NULL);
    thread_join(ui_thread, NULL);
    thread_join(gameplay_thread, NULL);
    delwin(snake_window);
    delwin(game_window);
    endwin();
    return 0;
}

thread_ret main_menu(void* args) {
    WINDOW* menu = newwin(0, 0, 0, 0);
    box(menu, 0, 0);
    const char welcome[] = "Welcome to Snake!";
    const int welcome_len = strlen(welcome);
    const char any_key[] = "Press any key to start...";
    const int any_len = strlen(any_key);
    const char author[] = "Made by HaywardHHayward";
    const int author_len = strlen(author);
    mvwprintw(menu, (LINES - 4) / 2, (COLS - welcome_len) / 2, welcome);
    mvwprintw(menu, (LINES - 2) / 2, (COLS - any_len) / 2, any_key);
    mvwprintw(menu, LINES - 2, (COLS - author_len) / 2, author);
    wrefresh(menu);
    wgetch(menu);
    werase(menu);
    wrefresh(menu);
    delwin(menu);
    return NULL;
}
