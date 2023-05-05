cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(unz-hid-pico)

pico_sdk_init()

add_executable(unz-hid-pico)

target_sources(unz-hid-pico PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/gamepad.c
        ${CMAKE_CURRENT_LIST_DIR}/src/hid_app.c
        ${CMAKE_CURRENT_LIST_DIR}/src/main.c
        ${CMAKE_CURRENT_LIST_DIR}/src/mouse.c
        )

target_include_directories(unz-hid-pico PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
        )

target_link_libraries(unz-hid-pico PUBLIC
        pico_stdlib
        tinyusb_host
        tinyusb_board
        )

pico_add_extra_outputs(unz-hid-pico)
