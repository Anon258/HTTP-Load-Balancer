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
    int timeouts;
    int nserved;
    std::map<std::string, int> urlhits;
    std::string server;

public:
    loadbalancer(std::string server) : nreq(0), nserved(0), timeouts(0), server(server) {}
    const std::shared_ptr<http_response> render(const http_request& req)
    {
        nreq ++;
        std::cout << "Started processing request number " << nreq << std::endl;
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
        header_map res_hds;
        std::string response;
        int rescode;

        minimal_httpclient internal;
        
        if (internal.request(server, method, body, &response, hds, &res_hds, rescode) == CURLE_OPERATION_TIMEDOUT)
        {
            timeouts++;
            std::cout << "Request with ID " << nreq << " timed out" << std::endl;
        }
        else
        {
            nserved++;
            std::cout << "Serving request with ID " << nreq << " now" << std::endl;
        }
        
        std::shared_ptr<http_response> res = std::shared_ptr<http_response>(new string_response(response, rescode));
        for (auto rh : res_hds)
        {
            res->with_header(rh.first, rh.second);
        }

        return res;
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
    webserver ws = create_webserver(8080).start_method(http::http_utils::THREAD_PER_CONNECTION);

    loadbalancer lb(server);

    curl_global_init(CURL_GLOBAL_ALL);

    ws.register_resource("/lb1", &lb);
    ws.register_resource("/lb2", &lb);
    ws.register_resource("/lb3", &lb);
    ws.register_resource("/lb4", &lb);
    ws.start(true);

    curl_global_cleanup();
        
    return 0;
}