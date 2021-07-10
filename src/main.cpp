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
#include <assert.h>

using namespace httpserver;

void custom_log( const std::string& url )
{
    std::ifstream ifd;
    ifd.open("../logs/url_hits/" + url, std::ios_base::in);
    int count = 0;
    if(ifd.good()) ifd >> count;
    ifd.close();
    count++;
    std::ofstream ofd;
    ofd.open("../logs/url_hits/" + url, std::ios_base::trunc);
    ofd << count;
    ofd.close();
}

const std::shared_ptr<http_response> internal_error_custom(const http_request& req)
{
	return std::shared_ptr<string_response>(new string_response("Server unreachable", 500, "text/plain"));
}

loadbalancer setup_config( const std::string config_file )
{
	std::ifstream ifd;
	ifd.open(config_file, std::ios_base::in);
	if(ifd.is_open())
	{
		std::string s;
		getline(ifd, s);
		
		if(s.substr(0,10) != "interval =")
		{	
			std::cerr << "Incorrect configuration format" << std::endl; 
			exit(0);
		}
		int interval = std::stoi(s.substr(11));
		std::vector< std::tuple < std::string, bool, std::string> > servers;
		
		getline(ifd, s);
		while( getline(ifd, s) )
		{
			std::vector<std::string> args;
			size_t pos;
			while ((pos = s.find(",")) != std::string::npos) {
				std::string token = s.substr(0, pos);
				args.push_back(token);
				s.erase(0, pos + 1);
			}
			args.push_back(s);
			if(args.size() == 2)
			{	
				assert(args[1] == "false");
				servers.push_back( std::make_tuple( args[0], true, "") );
			}
			else if(args.size() == 3)
			{
				assert(args[1] == "true");
				servers.push_back( std::make_tuple( args[0], true, args[2] ) );	
			}
			else
			{
				std::cerr << "Incorrect configuration format" << std::endl;
				exit(0);
			}
		}
		
		loadbalancer lb(servers, interval, "../logs/lblogs");
		return lb;
	}
	else
	{
		std::cerr << "File could not be opened" << std::endl;
		exit(0);
	}
}

int main(int argc, char **argv)
{
    if (argc != 2)
   {
        std::cout << "Usage : ./a.out config-file" << std::endl;
        return 1;
   }

    webserver ws = create_webserver(8080)
                    .start_method(http::http_utils::THREAD_PER_CONNECTION)
                    .internal_error_resource(internal_error_custom)
                    .log_access(custom_log);
    
    loadbalancer lb = setup_config(argv[1]);
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
