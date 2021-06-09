#include "http_client.hpp"
#include <iostream>

int main()
{
    curl_global_init(CURL_GLOBAL_ALL);
    http_client c1;
    std::string res;
    header_map hds;
    std::string body = "Lalalalalalalala lmfaoooooooooooooo!";
    hds["arg1"] = "data1";
    int code = c1.get("http://localhost:8080/hello",&res, hds);
    if( code != CURLE_OK )
    {
        std::cout << code << " " <<  c1.log_error();
    }
    else
    {
        std::cout << res << std::endl;
    }
    std::cout << c1.log_error();
    curl_global_cleanup();
    return 0;
}