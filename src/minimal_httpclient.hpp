#include <curl/curl.h>
#include <string>
#include <map>

#ifndef header_map
#define header_map std::map<std::string, std::string>
#endif

class minimal_httpclient
{
private:
    int nreq = 0;
    std::string err;
    bool log_en = false;
    static size_t write(void *buffer, size_t size, size_t nmemb, std::string *userp)
    {
        userp->append((char *)buffer, size * nmemb);
        return size * nmemb;
    }

public:
    void enable_logging() { log_en = true; }
    void disable_logging() { log_en = false; }
    const char *log_status() { return log_en ? "enabled" : "disabled"; }
    std::string log() { return err; }
    void free_log() { err.erase(); }

    /*Always set the Content-Type field yourself whenever necessary! Setting it to empty right now!*/
    CURLcode request(const std::string &url, const std::string &method, const std::string &body, std::string* response, const header_map &hds = header_map())
    {
        if (log_en)
            err += "Request ID " + std::to_string(nreq++) + "\n";
        CURL *hdl = curl_easy_init();

        curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(hdl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, method.c_str());

        curl_slist* hlist = NULL;

        if (hds.find("Content-Type") == hds.end())
        {
            hlist = curl_slist_append(hlist, "Content-Type:");
        }

        if (!hds.empty())
        {
            for(auto h : hds)
            {
                std::string header = h.first + ":" + h.second;
                hlist = curl_slist_append(hlist, header.c_str());
            }
        }
        curl_easy_setopt(hdl, CURLOPT_HTTPHEADER, hlist);
        
        CURLcode res = curl_easy_perform(hdl);
        if (log_en)
            err += curl_easy_strerror(res);
        if (log_en)
            err += "\n";

        curl_slist_free_all(hlist);
        curl_easy_cleanup(hdl);

        return res;
    }
};