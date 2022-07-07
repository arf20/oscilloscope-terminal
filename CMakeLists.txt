cmake_minimum_required(VERSION 3.0.0)
project(oscilloscope-terminal)
set(CMAKE_CXX_STANDARD 17)

add_executable(oscilloscope-terminal main.cpp)

#target_link_libraries(oscilloscope-terminal )

install(TARGETS oscilloscope-terminal RUNTIME DESTINATION bin)
