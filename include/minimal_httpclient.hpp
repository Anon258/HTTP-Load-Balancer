#ifndef __MINIMAL_HTTPCLIENT_HPP__
#define __MINIMAL_HTTPCLIENT_HPP__

#include <curl/curl.h>
#include <string>
#include <map>
#include <iostream>

#ifndef header_map
#define header_map std::map<std::string, std::string>
#endif

class minimal_httpclient
{
private:
    int nreq = 0;
    std::string err;
    bool log_en = false;
    static size_t write(void *buffer, size_t size, size_t nmemb, std::string *userp);
    static size_t write_h(void *buffer, size_t size, size_t nmemb, header_map *userp);

public:
    void enable_logging();
    void disable_logging();
    const char *log_status();
    const std::string& log();
    void free_log();

    /*Always set the Content-Type and Content-Length field yourself whenever necessary! Setting it to empty!*/
    CURLcode request
    (   const std::string &url,
        const std::string &method, 
        const std::string &body, 
        std::string* response, 
        const header_map &hds, 
        header_map* res_hds, 
        int& rescode
    );
};