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
     c1.enable_logging();
     int code = c1.get("http://localhost:8000",&res, hds);
     std::cout<<res<<std::endl;
     curl_global_cleanup();
     return 0;
 } 
