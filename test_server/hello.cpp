#include <httpserver.hpp>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

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

        return std::shared_ptr<http_response>(new string_response("Hello, World!"));
    }
};

int main(int argc, char** argv) 
{
    if (argc != 2)
    {
        std::cout << "Usage : ./a.out logfile-name" << std::endl; 
        return 1;
    }
    std::string lf (argv[1]);
    webserver ws = create_webserver(5656);

    hello_world_resource hwr(lf);
    ws.register_resource("/", &hwr);
    ws.start(true);
    
    return 0;
}