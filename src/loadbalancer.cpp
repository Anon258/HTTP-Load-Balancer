#include "loadbalancer.hpp"

void loadbalancer::healthcheck()
{
    while (true)
    {
        std::string logstring;
        for (int i = 0; i < totalServers; ++i)
        {
            std::string response;
            CURLcode hc_code = httpclient::request(servers[i], "HEAD", "", header_map(), &response);
            if (hc_code == CURLE_COULDNT_CONNECT)
            {
                asMutex.lock();
                activeServers.erase(i);
                asMutex.unlock();
            }
            else
            {
                logstring += servers[i] + ", ";
                asMutex.lock();
                activeServers.insert(i);
                asMutex.unlock();
            }
        }
        lfdMutex.lock();
        log_fd.open(LB_LOG, std::ios::app);
        log_fd << "Active server list updated :" << std::endl;
        log_fd << logstring << std::endl
               << std::endl;
        log_fd.close();
        lfdMutex.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

loadbalancer::loadbalancer (std::vector<std::string> urls, int interval) : 
servers(urls), interval(interval), hc_thread(&loadbalancer::healthcheck, this)
{
    lastUsedServer = -1;
    totalServers = urls.size();
    for (int i = 0; i < totalServers; ++i)
        activeServers.insert(i);
    lfdMutex.lock();
    log_fd.open(LB_LOG, std::ios::trunc);
    log_fd << "LOGS FOR LOADBALANCER :" << std::endl
           << std::endl;
    log_fd.close();
    lfdMutex.unlock();
}

const std::shared_ptr<http_response> loadbalancer::render(const http_request &req)
{
    std::map<std::string, std::string, http::header_comparator> headers = req.get_headers();
    header_map hds, res_hds;
    for (auto header : headers)
        hds.insert(header);
    std::string method = req.get_method(), body = req.get_content();

    std::string response;
    long rescode;

    lusMutex.lock();
    asMutex.lock();
    if (activeServers.empty())
    {
        lfdMutex.lock();
        log_fd.open(LB_LOG, std::ios::app);
        log_fd << "Request :" << std::endl;
        log_fd << "Method - " << method << std::endl;
        log_fd << "Client IP - " << req.get_requestor() << std::endl;
        log_fd << "Client Port - " << req.get_requestor_port() << std::endl;
        log_fd << "Server Forwarded To - "
               << "NIL" << std::endl
               << std::endl;
        log_fd.close();
        lfdMutex.unlock();
        lusMutex.unlock();
        asMutex.unlock();
        return std::shared_ptr<http_response>(new string_response("No servers available to handle current request", 503));
    }

    int inuse;
    if (activeServers.upper_bound(lastUsedServer) != activeServers.end())
        inuse = *activeServers.upper_bound(lastUsedServer);
    else
        inuse = *activeServers.begin();
    asMutex.unlock();
    lastUsedServer = inuse;
    lusMutex.unlock();

    lfdMutex.lock();
    log_fd.open(LB_LOG, std::ios::app);
    log_fd << "Request :" << std::endl;
    log_fd << "Method - " << method << std::endl;
    log_fd << "Client IP - " << req.get_requestor() << std::endl;
    log_fd << "Client Port - " << req.get_requestor_port() << std::endl;
    log_fd << "Server Forwarded To - " << servers[inuse] << std::endl
           << std::endl;
    log_fd.close();
    lfdMutex.unlock();

    // health check
    CURLcode hc_code = httpclient::request(servers[inuse], "HEAD", "", header_map(), &response);
    if (hc_code == CURLE_COULDNT_CONNECT)
    {
        asMutex.lock();
        activeServers.erase(inuse);
        asMutex.unlock();
        return std::shared_ptr<http_response>(new string_response(curl_easy_strerror(hc_code), 500));
    }
    response.clear();

    // request
    CURLcode res_code = httpclient::request(servers[inuse] + req.get_path(), method, body, hds, &response, &res_hds, &rescode);

    if (res_code != CURLE_OK)
        return std::shared_ptr<http_response>(new string_response(curl_easy_strerror(res_code), 500));

    std::shared_ptr<http_response> res = std::shared_ptr<http_response>(new string_response(response, (int)rescode));
    for (auto rh : res_hds)
    {
        res->with_header(rh.first, rh.second);
    }

    return res;
}