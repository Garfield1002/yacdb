cmake_minimum_required(VERSION 3.10)

# set the project name
project(YACDB-server C)

# add the executable
include_directories(parser/include)
include_directories(diskio/include)
include_directories(schema/include)

file(GLOB SOURCES "*/*.c")
add_executable(server server.c ${SOURCES})
add_executable(console main.c ${SOURCES})

# Compile with option -pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)