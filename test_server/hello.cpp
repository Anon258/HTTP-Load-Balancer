#include <httpserver.hpp>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace httpserver;

class hello_world_resource : public http_resource {
private:
    int nreq;
    std::string logfile;
public:
    hello_world_resource(std::string lf) : logfile(lf), nreq(0) {}
    const std::shared_ptr<http_response> render(const http_request& req)
    {
        nreq ++;
        std::string log = "Request Number  : " + std::to_string(nreq) + "\n";
        log += "URL Requested : " + req.get_path() + "\n";
        log += "Method : " + req.get_method() + "\n";
        std::map<std::string, std::string, http::header_comparator> headers = req.get_headers();
        log += "Headers : \n";
        for (auto h : headers)
        {
            log += "  " + h.first + " : " + h.second + "\n";
        } 
        log += "Body : " + req.get_content() + "\n\n";

        std::ofstream fd;
        fd.open(logfile, std::ios_base::app);
        fd << log;
        fd.close();

        std::cout << log;

        if (rand()%100 > 75)
        {
            usleep(10000);
        }

        return std::shared_ptr<http_response>(new string_response("Hello, World!"));
    }
};

int main(int argc, char** argv) 
{
    srand(time(0));

    if (argc != 2)
    {
        std::cout << "Usage : ./a.out logfile-name" << std::endl; 
        return 1;
    }
    std::string lf (argv[1]);
    webserver ws = create_webserver(5656).start_method(http::http_utils::THREAD_PER_CONNECTION);

    hello_world_resource hwr(lf);
    ws.register_resource("/", &hwr);
    ws.start(true);
    
    return 0;
}