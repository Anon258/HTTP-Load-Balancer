#include "loadbalancer.hpp"

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
    std::vector<std::string> urls;

    std::ifstream config_fd;
    config_fd.open("../config.txt");
    config_fd >> interval;
    std::string url;
    while (config_fd >> url)
        urls.push_back(url);
    config_fd.close();

    for (int i = 1; i < argc; ++i)
    {
        urls.push_back(std::string(argv[i]));
    }

    webserver ws = create_webserver(8080)
                       .start_method(http::http_utils::THREAD_PER_CONNECTION)
                       .log_access(custom_log);

    loadbalancer lb(urls, interval);

    curl_global_init(CURL_GLOBAL_ALL);

    ws.register_resource("/", &lb, true);
    ws.start(true);

    curl_global_cleanup();
}