cmake_minimum_required ( VERSION 3.13 )
project(loadbalancer)

message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")

# loadbuffer
add_executable(lb 
	src/main.cpp
	src/minimal_http_client.cpp)

target_include_directories(lb include)
find_package( CURL REQUIRED )
target_link_libraries(lb httpserver CURL::libcurl)

# client
add_executable(client 
	test_client/client.cpp
	src/http_client.cpp)

target_include_directories(client include)
target_link_libraries(client CURL::libcurl)

# server
add_executable(server test_server/hello.cpp)
target_link_libraries(server httpserver)
