#include "http_client.hpp"
#include <iostream>

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    http_client c1;
    std::string res;
    header_map hds;
    hds["arg1"] = "data1";
    int code = c1.putfile("http://localhost:8080/hello", std::string("index.html"), &res , hds);
    std::cout << c1.log_error();
    std::cout << res << std::endl;
    curl_global_cleanup();
    return 0;
}