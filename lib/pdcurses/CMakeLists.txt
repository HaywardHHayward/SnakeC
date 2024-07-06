cmake_minimum_required(VERSION 3.29)
project(PDCurses C)

set(CMAKE_C_STANDARD 11)

add_library(PDCurses STATIC
        curses.h
        curspriv.h
        addch.c
        addchstr.c
        addstr.c
        attr.c
        beep.c
        bkgd.c
        border.c
        clear.c
        color.c
        debug.c
        delch.c
        deleteln.c
        getch.c
        getstr.c
        getyx.c
        inch.c
        inchstr.c
        initscr.c
        inopts.c
        insch.c
        insstr.c
        instr.c
        kernel.c
        keyname.c
        mouse.c
        move.c
        outopts.c
        overlay.c
        pad.c
        panel.c
        panel.h
        pdcclip.c
        pdcdisp.c
        pdcgetsc.c
        pdckbd.c
        pdcscrn.c
        pdcsetsc.c
        pdcutil.c
        pdcwin.h
        printw.c
        refresh.c
        scanw.c
        scr_dump.c
        scroll.c
        slk.c
        termattr.c
        touch.c
        util.c
        window.c
        common/acs437.h
        common/acsgr.h
        common/acsuni.h
        common/iconbmp.h)

target_include_directories(PDCurses
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/common
)