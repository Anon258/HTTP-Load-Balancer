#include "loadbalancer.hpp"
#include <httpserver.hpp>
#include <string>
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <ctime>			// For timer
#include <future>			// For std::async call
#include <vector>
#include <tuple>

using namespace httpserver;

void custom_log( const std::string& url )
{
    std::ifstream ifd;
    ifd.open("../logs/" + url, std::ios_base::in);
    int count = 0;
    if(ifd.good()) ifd >> count;
    ifd.close();
    count++;
    std::ofstream ofd;
    ofd.open("../logs/" + url, std::ios_base::trunc);
    ofd << count;
    ofd.close();
}

const std::shared_ptr<http_response> internal_error_custom(const http_request& req)
{
	return std::shared_ptr<string_response>(new string_response("Server unreachable", 500, "text/plain"));
}

int main(int argc, char **argv)
{
//    if (argc > 2)
//   {
//        std::cout << "Usage : ./a.out" << std::endl;
//        return 1;
//   }
//
//    std::string server(argv[1]);

    webserver ws = create_webserver(8080)
                    .start_method(http::http_utils::THREAD_PER_CONNECTION)
                    .internal_error_resource(internal_error_custom)
                    .log_access(custom_log);

    std::vector< std::tuple< std::string, bool, std::string> > servers;
    servers.push_back( std::make_tuple( "localhost:5656", true, "localhost:5656") );
    servers.push_back( std::make_tuple( "localhost:5657", true, "localhost:5657") );
    
    
    loadbalancer lb( servers , 5);
    auto health = std::async(std::launch::async, [&lb](){return lb.health_check(); });

    curl_global_init(CURL_GLOBAL_ALL);

    ws.register_resource("/lb1", &lb);
    ws.register_resource("/lb2", &lb);
    ws.register_resource("/lb3", &lb);
    ws.register_resource("/lb4", &lb);
    ws.start(true);

    curl_global_cleanup();

    return 0;
}
