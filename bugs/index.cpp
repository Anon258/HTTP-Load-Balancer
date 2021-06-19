#include "../src/minimal_httpclient.hpp"
#include <iostream>
#include <unistd.h>

/* Run along with the test server to observe weird behaviour in i variable*/

int main()
{
    std::string res;
    int rescode;
    curl_global_init(CURL_GLOBAL_ALL);
    minimal_httpclient c;
    int i = 0;

    // Should update

    std::cout << "Request ID : " << ++i << std::endl;
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();

    std::cout << "Request ID : " << ++i << std::endl;
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();

    std::cout << "Request ID : " << ++i << std::endl;
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    std::cout << "Log : " << c.log() << std::endl;
    res.clear();

    header_map r_hds;

    // Doesn't update
    std::cout << "Request ID : " << ++i << std::endl;
    c.request("http://localhost:8080/lb1", "GET", "", &res, header_map(), &r_hds, rescode);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    res.clear();

    for (auto h : r_hds)
    {
        std::cout << h.first << " : " << h.second;
    }
    r_hds.clear();
    usleep(500000);
    std::cout << std::endl;

    std::cout << "Request ID : " << ++i << std::endl;
    c.request("http://localhost:8080/lb2", "GET", "", &res, header_map(), &r_hds, rescode);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    res.clear();

    for (auto h : r_hds)
    {
        std::cout << h.first << " : " << h.second;
    }
    r_hds.clear();
    usleep(500000);
    std::cout << std::endl;

    std::cout << "Request ID : " << ++i << std::endl;
    c.request("http://localhost:8080/lb3", "GET", "", &res, header_map(), &r_hds, rescode);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    res.clear();

    for (auto h : r_hds)
    {
        std::cout << h.first << " : " << h.second;
    }
    r_hds.clear();
    usleep(500000);
    std::cout << std::endl;

    std::cout << "Request ID : " << ++i << std::endl;
    c.request("http://localhost:8080/lb3", "GET", "", &res, header_map(), &r_hds, rescode);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    res.clear();

    for (auto h : r_hds)
    {
        std::cout << h.first << " : " << h.second;
    }
    r_hds.clear();
    usleep(500000);
    std::cout << std::endl;

    std::cout << "Request ID : " << ++i << std::endl;
    c.request("http://localhost:8080/lb3", "GET", "", &res, header_map(), &r_hds, rescode);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    res.clear();

    for (auto h : r_hds)
    {
        std::cout << h.first << " : " << h.second;
    }
    r_hds.clear();
    usleep(500000);
    std::cout << std::endl;

    std::cout << "Request ID : " << ++i << std::endl;
    c.request("http://localhost:8080/lb3", "GET", "", &res, header_map(), &r_hds, rescode);
    std::cout << "Response : " << res << std::endl;
    std::cout << "Code : " << rescode << std::endl;
    res.clear();

    for (auto h : r_hds)
    {
        std::cout << h.first << " : " << h.second;
    }
    r_hds.clear();
    usleep(500000);
    std::cout << std::endl;

    curl_global_cleanup();
}