#include "http_client.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    std::string res;
    long rescode;
    curl_global_init(CURL_GLOBAL_ALL);
    http_client c;
    c.enable_logging();

    srand(time(0));

    for (int index = 0; index < 10; ++index)
    {
        std::cout << "Request ID : " << index + 1 << std::endl;
        c.put("http://localhost:8080/lb" + std::to_string((rand()&3) + 1), rescode, "hello", &res);
        std::cout << "Response : " << res << std::endl;
        std::cout << "Code : " << rescode << std::endl;
        std::cout << "Log : " << c.log() << std::endl;
        res.clear();
        c.free_log();
    }

    curl_global_cleanup();
}
