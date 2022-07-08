cmake_minimum_required(VERSION 3.0.0)
project(oscilloscope-terminal)
set(CMAKE_CXX_STANDARD 17)

add_executable(oscilloscope-terminal terminal.cpp graphics.cpp main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(oscilloscope-terminal Threads::Threads SDL2main SDL2 GL)

install(TARGETS oscilloscope-terminal RUNTIME DESTINATION bin)
