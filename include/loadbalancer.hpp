#include "httpclient.hpp"
#include <httpserver.hpp>

#include <vector>
#include <set>

#include <mutex>
#include <thread>
#include <chrono>

#include <fstream>

#define LB_LOG "../logs/lb_logs.txt"

using namespace httpserver;

class loadbalancer : public http_resource
{
private:
    int interval;
    std::vector<std::string> servers;
    int lastUsedServer, totalServers;
    std::set<int> activeServers;
    std::mutex lusMutex, asMutex, lfdMutex;
    std::thread hc_thread;
    std::ofstream log_fd;

    void healthcheck();

public:
    loadbalancer(std::vector<std::string> urls, int interval);
    const std::shared_ptr<http_response> render(const http_request &req);
};