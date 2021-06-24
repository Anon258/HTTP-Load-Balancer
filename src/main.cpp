#include "../include/minimal_httpclient.hpp"
#include <httpserver.hpp>
#include <string>
#include <unordered_map>

#include <iostream>
#include <fstream>
#include <ctime>			// For timer
#include <future>			// For std::async call

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

class loadbalancer : public http_resource
{
  private:
    
    int nreq, timeouts, nserved;
    std::string server;
    bool active;
    std::time_t start, curr;

    void send_health()
    {
	
        minimal_httpclient internal;
        header_map hds, res_hds;
        std::string response;
        long rescode;
	if (internal.request(server, "GET", "Regular Health Checkup", &response, hds, &res_hds, rescode) == CURLE_OK)
	{
		std::cout << "Health Check Successful" << std::endl;
		active = true;
	}
	else
	{
		std::cout << "Health Check Failed" << std::endl;
		active = false;
	}
    }


  public:
    loadbalancer(std::string server) :
    nreq(0), nserved(0), timeouts(0), server(server), active(false) {}
    
    void health_check()
    {
	start = std::time(nullptr);
    	while(1)
	{
		curr = std::time(nullptr);
		if(curr - start >= 5)
		{
			send_health();
			start = curr;
		}
		
	}
    }

    const std::shared_ptr<http_response> render(const http_request &req)
    {
        nreq++;
        std::cout << "Started processing request number " << nreq << std::endl;

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
        long rescode;

        minimal_httpclient internal;
	if (active)
	{
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

		std::shared_ptr<http_response> res = std::shared_ptr<http_response>(new string_response(response, (int)rescode));
		for (auto rh : res_hds)
		{
			res->with_header(rh.first, rh.second);
		}

		return res;
	}
	else
	{
		return std::shared_ptr<http_response>(new string_response("Server is unreachable", 500));		
	}
    }
};

//bool loadbalancer::active = false;
//std::time_t loadbalancer::start = std::time(nullptr);
//std::time_t loadbalancer::curr = std::time(nullptr);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage : ./a.out server-url" << std::endl;
        return 1;
    }
    std::string server(argv[1]);

    webserver ws = create_webserver(8080)
                    .start_method(http::http_utils::THREAD_PER_CONNECTION)
                    .log_access(custom_log);

    loadbalancer lb(server);
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
