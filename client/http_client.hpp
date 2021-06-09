#include <map>
#include <string>
#include <curl/curl.h>

#define header_map std::map<std::string, std::string>
#define CURL_BAD_HANDLE -1
#define CURL_FILE_ERR -2

class http_client
{
private:
    std::string err;
    static size_t write(void *buffer, size_t size, size_t nmemb, std::string* userp)
    {
        userp->append((char*) buffer, size*nmemb);
        return size*nmemb;
    }
public:
    int get(std::string url,std::string* response, header_map headers);
    std::string log_error() { return err; }
    void free_log() { err.erase(); }
};

int http_client::get(std::string url, std::string* response, header_map headers = header_map())
{
    err += "get() :\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    
    // header building
    curl_slist* hds = NULL;
    if (!headers.empty())
    {
        for(auto h : headers)
        {
            std::string header = h.first + ":" + h.second;
            hds = curl_slist_append(hds, header.c_str());
        }
        curl_easy_setopt(hdl, CURLOPT_HTTPHEADER, hds);
    }

    // perform
    CURLcode res = curl_easy_perform(hdl);
    if (res != CURLE_OK)
    {
        err += curl_easy_strerror(res);
        err += "\n\n";
    }

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}