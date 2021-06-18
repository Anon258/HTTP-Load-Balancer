cmake_minimum_required ( VERSION 3.13 )
project(loadbalancer)

message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")
set(SOURCE_FILES main.cpp)
add_executable(lb ${SOURCE_FILES})
find_package( CURL REQUIRED )
target_link_libraries(lb httpserver CURL::libcurl)