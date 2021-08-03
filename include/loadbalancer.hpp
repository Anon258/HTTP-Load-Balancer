#ifndef _LOAD_BALANCER_HPP_
#define _LOAD_BALANCER_HPP_

#include "httpclient.hpp"
#include "logger.hpp"
#include <httpserver.hpp>

#include <vector>
#include <set>

#include <mutex>
#include <thread>
#include <chrono>

using namespace httpserver;

class loadbalancer : public http_resource
{
private:
    int interval;

    std::vector<std::string> servers;
    std::vector<std::string> hc_urls;
    std::vector<bool> strict_hc;

    int lastUsedServer, totalServers;
    std::set<int> activeServers;
    std::mutex asMutex;
    std::thread hc_thread;
    logger* lg;

    void healthcheck();

public:
    loadbalancer(std::vector<std::string> urls, std::vector<bool> strict_hc, std::vector<std::string> hc_urls, int interval);
    inline void add_logger(logger* lgptr) { lg = lgptr; }
    const std::shared_ptr<http_response> render(const http_request &req);
};

#endif