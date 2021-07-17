cmake_minimum_required ( VERSION 3.13 )
project (http-loadbalancer)

message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")

find_package( CURL REQUIRED )

add_executable(lb src/loadbalancer.cpp src/httpclient.cpp src/main.cpp)
target_include_directories(lb PRIVATE include)
target_link_libraries(lb httpserver CURL::libcurl pthread)