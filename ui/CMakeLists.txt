cmake_minimum_required(VERSION 3.10)

# set the project name
project(Parser)

# add the executable
include_directories(ui-include)
# add_executable(maze main.cpp sdlHandle.cpp piece.cpp slotGrid.cpp)

file(GLOB SOURCES "*.c")
add_executable(parser ${SOURCES})