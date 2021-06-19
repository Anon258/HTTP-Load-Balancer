#include "minimal_httpclient.hpp"

static size_t minimal_httpclient::write(void *buffer, size_t size, size_t nmemb, std::string *userp)
{
    userp->append((char *)buffer, size * nmemb);
    return size * nmemb;
}
static size_t minimal_httpclient::write_h(void *buffer, size_t size, size_t nmemb, header_map *userp)
{
    std::string header;
    header.append((char*) buffer, size*nmemb);
    size_t idx = header.find(':');
    if (idx != std::string::npos)
    {
        std::string opt, val;
        opt = header.substr(0, idx);
        val = header.substr(idx+1);
        userp->insert( std::pair<std::string, std::string> (opt, val) );
    }
    return size*nmemb;
}

CURLcode minimal_httpclient::request
(   const std::string &url,
    const std::string &method, 
    const std::string &body, 
    std::string* response, 
    const header_map &hds, 
    header_map* res_hds, 
    int& rescode
)
{
    if (log_en)
        err += "Request ID " + std::to_string(nreq++) + "\n";
    CURL *hdl = curl_easy_init();

    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(hdl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(hdl, CURLOPT_HEADERFUNCTION, write_h);
    curl_easy_setopt(hdl, CURLOPT_HEADERDATA, res_hds);

    curl_slist* hlist = NULL;

    if (hds.find("Content-Type") == hds.end())
    {
        hlist = curl_slist_append(hlist, "Content-Type:");
    }
    if (hds.find("Content-Length") == hds.end())
    {
        hlist = curl_slist_append(hlist, "Content-Length:");
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

    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);

    if (log_en)
        err += curl_easy_strerror(res);
    if (log_en)
        err += "\n";

    curl_slist_free_all(hlist);
    curl_easy_cleanup(hdl);

    return res;
}

void minimal_httpclient::enable_logging() { log_en = true; }
void minimal_httpclient::disable_logging() { log_en = false; }
const char* minimal_httpclient::log_status() { return log_en ? "enabled" : "disabled"; }
const std::string& minimal_httpclient::log() { return err; }
void minimal_httpclient::free_log() { err.clear(); }