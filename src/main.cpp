#include "minimal_httpclient.hpp"
#include <httpserver.hpp>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

using namespace httpserver;

class loadbalancer : public http_resource {
private:
    int nreq;
    int nserved;
    int timeouts;
    std::map<std::string, int> urlhits;
    minimal_httpclient internal;
    std::string server;
public:
    loadbalancer(std::string server) : nreq(0), nserved(0), timeouts(0), server(server) {}
    const std::shared_ptr<http_response> render(const http_request& req)
    {
        nreq ++;
        std::string url = req.get_path();
        if ( urlhits.find(url) == urlhits.end())
            urlhits[url] = 0;
        urlhits[url] ++;

        std::map<std::string, std::string, http::header_comparator> headers = req.get_headers();
        header_map hds;
        for (auto h : headers)
        {
            hds.insert(h);
        }
        std::string method = req.get_method(); 
        std::string body = req.get_content();

        std::string response;

        CURLcode res = internal.request(server, method, body, &response, hds);

        return std::shared_ptr<http_response>(new string_response(response), (int)res);
    }
};

int main(int argc, char** argv) 
{
    if (argc != 2)
    {
        std::cout << "Usage : ./a.out server-url" << std::endl; 
        return 1;
    }
    std::string server (argv[1]);
    webserver ws = create_webserver(8080);

    loadbalancer lb(server);
    ws.register_resource("/lb1", &lb);
    ws.register_resource("/lb2", &lb);
    ws.register_resource("/lb3", &lb);
    ws.register_resource("/lb4", &lb);
    ws.start(true);
    
    return 0;
}
