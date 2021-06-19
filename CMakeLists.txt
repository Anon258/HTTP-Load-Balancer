cmake_minimum_required ( VERSION 3.13 )
project(loadbalancer)

message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")

add_executable(lb src/main.cpp)
find_package( CURL REQUIRED )
target_link_libraries(lb httpserver CURL::libcurl)

add_executable(client test_client/client.cpp)
target_link_libraries(client CURL::libcurl)

add_executable(server test_server/hello.cpp)
target_link_libraries(server httpserver)
