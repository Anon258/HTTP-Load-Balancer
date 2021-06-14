cmake_minimum_required(VERSION 3.10)
project(hello_server)

message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
set(SOURCE_FILES hello.cpp)
add_executable(hello_server ${SOURCE_FILES})
target_link_libraries(hello_server httpserver)
