#include "loadbalancer.hpp"
#include "logger.hpp"

#include <string>
#include <fstream>
#include <vector>

void custom_log(const std::string &url)
{
    std::ifstream ifd;
    ifd.open("../logs/url_hits/" + url, std::ios_base::in);
    int count = 0;
    if (ifd.good())
        ifd >> count;
    ifd.close();
    count++;
    std::ofstream ofd;
    ofd.open("../logs/url_hits/" + url, std::ios_base::trunc);
    ofd << count;
    ofd.close();
}

int main(int argc, char *argv[])
{
    int interval;
    std::vector<std::string> urls, hc_urls;
    std::vector<bool> strict_hc;

    std::ifstream config_fd;
    config_fd.open("../config.txt");
    config_fd >> interval;
    config_fd.ignore();
    std::string url;
    while (std::getline(config_fd, url))
    {
        int pos = url.find(" ");
        if (pos == std::string::npos)
        {
            urls.push_back(url);
            hc_urls.push_back(url);
            strict_hc.push_back(false);
        }
        else
        {
            urls.push_back(url.substr(0,pos));
            hc_urls.push_back(url.substr(pos+1));
            strict_hc.push_back(true);
        }
    }
    config_fd.close();

    webserver ws = create_webserver(8080)
                       .start_method(http::http_utils::THREAD_PER_CONNECTION)
                       .log_access(custom_log);

    loadbalancer lb(urls, strict_hc, hc_urls, interval);
    lb.add_logger(new logger("../logs/lb_logs.txt"));

    curl_global_init(CURL_GLOBAL_ALL);

    ws.register_resource("/", &lb, true);
    ws.start(true);

    curl_global_cleanup();
}