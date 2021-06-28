#ifndef __LOADBALANCER_HPP__
#define __LOADBALANCER_HPP__

#include "minimal_httpclient.hpp"
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

class loadbalancer : public http_resource
{
  private:
    
    int nreq, timeouts, nserved;
    std::vector< std::tuple< std::string, bool, std::string> > servers;	// Server name, active, dedicated url (if not exist then empty) 
    int interval;
    std::time_t start, curr;
    int nservers;
    int next_serv;

    void send_health(std::tuple<std::string, bool, std::string> &sv);
    std::shared_ptr<http_response> send_req(const http_request &req, std::string server);
    std::string round_robin_select();


  public:
    inline loadbalancer(std::vector< std::tuple<std::string, bool, std::string> > servers, int interval) :
    nreq(0), nserved(0), timeouts(0), servers(servers), interval(interval), nservers(servers.size()), next_serv(0) {}
    
    void health_check();
    const std::shared_ptr<http_response> render(const http_request &req);
};

#endif
