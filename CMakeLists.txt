cmake_minimum_required(VERSION 3.0.0)
project(oscilloscope-terminal)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")

add_executable(oscilloscope-terminal audio.cpp terminal.cpp graphics.cpp main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(oscilloscope-terminal Threads::Threads SDL2main SDL2 GL)

#target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g -fsanitize=address)
#target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)

install(TARGETS oscilloscope-terminal RUNTIME DESTINATION bin)
