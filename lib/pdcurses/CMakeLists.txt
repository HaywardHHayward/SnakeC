cmake_minimum_required(VERSION 3.29)
project(PDCurses C)

set(CMAKE_C_STANDARD 17)

add_library(PDCurses STATIC
            include/curses.h
            include/panel.h
            src/curspriv.h
            src/addch.c
            src/addchstr.c
            src/addstr.c
            src/attr.c
            src/beep.c
            src/bkgd.c
            src/border.c
            src/clear.c
            src/color.c
            src/debug.c
            src/delch.c
            src/deleteln.c
            src/getch.c
            src/getstr.c
            src/getyx.c
            src/inch.c
            src/inchstr.c
            src/initscr.c
            src/inopts.c
            src/insch.c
            src/insstr.c
            src/instr.c
            src/kernel.c
            src/keyname.c
            src/mouse.c
            src/move.c
            src/outopts.c
            src/overlay.c
            src/pad.c
            src/panel.c
            src/pdcclip.c
            src/pdcdisp.c
            src/pdcgetsc.c
            src/pdckbd.c
            src/pdcscrn.c
            src/pdcsetsc.c
            src/pdcutil.c
            src/pdcwin.h
            src/printw.c
            src/refresh.c
            src/scanw.c
            src/scr_dump.c
            src/scroll.c
            src/slk.c
            src/termattr.c
            src/touch.c
            src/util.c
            src/window.c
            src/common/acs437.h
            src/common/acsgr.h
            src/common/acsuni.h
            src/common/iconbmp.h
            src/curspriv.h)

target_include_directories(PDCurses
                           PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/common ${CMAKE_CURRENT_SOURCE_DIR}/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/common
)