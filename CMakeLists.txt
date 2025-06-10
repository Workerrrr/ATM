cmake_minimum_required(VERSION 3.29)
project(CLion_Project C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(CLion_Project
        main.c
        balance.c
        check.c

        atm_head.h
        init_window.h
        button_clicked.h)

target_link_libraries(CLion_Project ${GTK3_LIBRARIES})