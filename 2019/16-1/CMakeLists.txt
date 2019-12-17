# CMakeLists.txt for Puzzle 16-1 for Advent of Code 2019
#
# © 2019 by Richard Walters

cmake_minimum_required(VERSION 3.8)
set(This aoc16_1)

set(Sources
    src/main.cpp
)

add_executable(${This} ${Sources})
set_target_properties(${This} PROPERTIES
    FOLDER 2019
)

target_link_libraries(${This} PUBLIC
)

if(UNIX AND NOT APPLE)
    target_link_libraries(${This} PRIVATE
        -static-libstdc++
    )
endif(UNIX AND NOT APPLE)
