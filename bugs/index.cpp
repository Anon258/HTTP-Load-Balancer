#include "../src/http_client.hpp"
#include <iostream>

/* Run along with the test server to observe weird behaviour in i variable*/

int main()
{
    std::string res;
    int rescode;
    curl_global_init(CURL_GLOBAL_ALL);
    http_client c;
    c.enable_logging();
    int i = 0;

    // Should update

    std::cout << "Request ID : " << ++i << std::endl;
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();
    c.free_log();

    std::cout << "Request ID : " << ++i << std::endl;
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();
    c.free_log();

    std::cout << "Request ID : " << ++i << std::endl;
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();
    c.free_log();

    // Doesn't update
    std::cout << "Request ID : " << ++i << std::endl;
    c.get("http://localhost:5656/", rescode, &res);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();
    c.free_log();

    std::cout << "Request ID : " << ++i << std::endl;
    c.get("http://localhost:5656/", rescode, &res);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();
    c.free_log();

    std::cout << "Request ID : " << ++i << std::endl;
    c.get("http://localhost:5656/", rescode, &res);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();
    c.free_log();

    curl_global_cleanup();
}