cmake_minimum_required ( VERSION 3.13 )
project(loadbalancer)

message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")

find_package( CURL REQUIRED )

# loadbuffer
add_executable(lb src/main.cpp src/minimal_httpclient.cpp)
target_include_directories(lb PRIVATE include)
target_link_libraries(lb httpserver CURL::libcurl)
target_link_libraries(lb pthread)

# client
add_executable(client test_client/client.cpp src/http_client.cpp)
target_include_directories(client PRIVATE include)
target_link_libraries(client CURL::libcurl)

# server
add_executable(server test_server/hello.cpp)
find_package(Threads REQUIRED)
target_link_libraries(server httpserver)
