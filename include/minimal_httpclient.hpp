#ifndef __MINIMAL_HTTPCLIENT_HPP__
#define __MINIMAL_HTTPCLIENT_HPP__

#include <curl/curl.h>
#include <string>
#include <map>

#ifndef header_map
#define header_map std::map<std::string, std::string>
#endif

class minimal_httpclient
{
private:
    unsigned int nreq = 0;
    inline static size_t write(void *buffer, size_t size, size_t nmemb, std::string *userp)
    {
        userp->append((char *)buffer, size * nmemb);
        return size * nmemb;
    }
    inline static size_t write_h(void *buffer, size_t size, size_t nmemb, header_map *userp)
    {
        std::string header;
        header.append((char *)buffer, size * nmemb);
        size_t idx = header.find(':');
        if (idx != std::string::npos)
        {
            std::string opt, val;
            opt = header.substr(0, idx);
            val = header.substr(idx + 1);
            userp->insert(std::pair<std::string, std::string>(opt, val));
        }
        return size * nmemb;
    }
    inline unsigned int requests_made() { return nreq; }

public:
    /*Always set the Content-Type and Content-Length field yourself whenever necessary! Setting it to empty!*/
    CURLcode request (
        const std::string &url,
        const std::string &method,
        const std::string &body,
        std::string *response,
        const header_map &hds,
        header_map *res_hds,
        long &rescode
    );
};

#endif