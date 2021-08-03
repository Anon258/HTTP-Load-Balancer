#include "loadbalancer.hpp"

void loadbalancer::healthcheck()
{
    while (true)
    {
        std::string logstring;
        for (int i = 0; i < totalServers; ++i)
        {
            std::string response;
            long rescode;
            CURLcode hc_code = httpclient::request(hc_urls[i], "HEAD", "", header_map(), &response, nullptr, &rescode);
            if ( strict_hc[i] ? (rescode != 200) : (hc_code == CURLE_COULDNT_CONNECT) )
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
        if (lg) lg->add_log("Active Server List : " + logstring + "\n");
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

loadbalancer::loadbalancer (std::vector<std::string> urls, std::vector<bool> strict_hc, std::vector<std::string> hc_urls, int interval) : 
servers(urls), strict_hc(strict_hc), hc_urls(hc_urls), interval(interval), hc_thread(&loadbalancer::healthcheck, this)
{
    lastUsedServer = -1;
    totalServers = urls.size();
    for (int i = 0; i < totalServers; ++i)
        activeServers.insert(i);
    lg = nullptr;
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

    std::string log = "";
    log += "Request:";
    log += "Method-" + method + ", ";
    log += "Client_IP-" + req.get_requestor() + ", ";

    asMutex.lock();
    if (activeServers.empty())
    {
        asMutex.unlock();
        log += "Backend_Server-NIL\n";
        if(lg) lg->add_log(log);
        return std::shared_ptr<http_response>(new string_response("<h1>Unavailable</h1><b>No backend servers can handle the request</b>", 503));
    }

    int inuse;
    if (activeServers.upper_bound(lastUsedServer) != activeServers.end())
        inuse = *activeServers.upper_bound(lastUsedServer);
    else
        inuse = *activeServers.begin();
    lastUsedServer = inuse;
    asMutex.unlock();

    log += "Backend_Server-" + servers[inuse] + "\n";
    if(lg) lg->add_log(log);

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