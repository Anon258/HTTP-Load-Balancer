#include "../src/http_client.hpp"
#include <iostream>

int main()
{
    std::string res;
    int rescode;
    curl_global_init(CURL_GLOBAL_ALL);
    http_client c;
    c.enable_logging();
    int i = 0;

    std::cout << "Request ID : " << ++i << std::endl;
    c.putfile("http://localhost:8080/lb1", rescode, "../../CP/TCs/long_text.txt");
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();
    c.free_log();

    curl_global_cleanup();
}