#include "minimal_httpclient.hpp"

CURLcode minimal_httpclient::request( const std::string &url, const std::string &method, const std::string &body, 
    std::string* response, const header_map &hds, header_map* res_hds, long& rescode)
{
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
    curl_easy_setopt(hdl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(hdl, CURLOPT_TIMEOUT_MS, 3000L);
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    
    curl_slist_free_all(hlist);
    curl_easy_cleanup(hdl);

    return res;
}
