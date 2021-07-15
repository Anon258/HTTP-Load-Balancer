#ifndef __HTTPCLIENT_HPP__
#define __HTTPCLIENT_HPP__

#include <curl/curl.h>
#include <string>
#include <map>

typedef std::map<std::string, std::string> header_map;

class httpclient
{
private:
    inline static size_t write(void *buffer, size_t size, size_t nmemb, std::string *userp);
    inline static size_t write_h(void *buffer, size_t size, size_t nmemb, header_map *userp);

public:
    /*Always set the Content-Type and Content-Length field yourself whenever necessary! 
    Will be setting it to empty if these are not passed in the header map!*/
    static CURLcode request (
        const std::string &url,
        const std::string &method,
        const std::string &body,
        const header_map &headers,
        std::string *response,
        header_map *res_headers = nullptr,
        long *res_code = nullptr
    );
};

#endif