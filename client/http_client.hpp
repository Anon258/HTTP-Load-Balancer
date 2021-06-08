#include <map>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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
    static size_t writef(void *buffer, size_t size, size_t nmemb, std::ofstream* userp)
    {
        userp->write((char*) buffer, size*nmemb);
        return size*nmemb;
    }
    static size_t read(void *buffer, size_t size, size_t nmemb, std::stringstream* userp)
    {
        userp->read((char*)buffer, size*nmemb);
        return userp->gcount();
    }
    static size_t readf(void *buffer, size_t size, size_t nmemb, FILE* userp)
    {
        size_t num = fread(buffer, size, nmemb, userp);
        return num;
    }
public:
    /* Notation : 
        reqtype(response)_(capture), both default to a string variable
        do not give a string variable for custom request if you do not need the response
    */
    int get(std::string url,std::string& response, header_map headers = header_map());
    int getf(std::string url, std::string filename, header_map headers = header_map());
    int put(std::string url, std::string data, header_map headers = header_map());
    int put_f(std::string url, std::string filename,header_map headers = header_map());
    // int post();
    // int postf();
    // will handle later
    int custom(std::string url, std::string type, std::string body, header_map headers = header_map());
    int custom(std::string url, std::string type, std::string body, std::string& response, header_map headers = header_map());
    int customf(std::string url, std::string type, std::string filename, std::string& response, header_map headers = header_map());
    int customf_f(std::string url, std::string type, std::string filename, std::string resfile, header_map headers = header_map());
    int custom_f(std::string url, std::string type, std::string body, std::string resfile, header_map headers = header_map());
    std::string log_error() { return err; }
    void free_log() { err.erase(); }
};

int http_client::get(std::string url,std::string& response, header_map headers = header_map())
{
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url);
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
    char errbuf[CURL_ERROR_SIZE];
    errbuf[0] = 0;
    curl_easy_setopt(hdl, CURLOPT_ERRORBUFFER, errbuf);
    int res = curl_easy_perform(hdl);
    if (strlen(errbuf))
    {
        err.append(errbuf, strlen(errbuf));
        err += "\n\n";
    }

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return res;
}

int http_client::getf(std::string url,std::string filename, header_map headers = header_map())
{
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // opening file
    std::ofstream file;
    file.open(filename);
    if (!file)
    {
        err += "Could not open file\n\n";
        return CURL_FILE_ERR;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url);
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, writef);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, &file);
    
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
    char errbuf[CURL_ERROR_SIZE];
    errbuf[0] = 0;
    curl_easy_setopt(hdl, CURLOPT_ERRORBUFFER, errbuf);
    int res = curl_easy_perform(hdl);
    if (strlen(errbuf))
    {
        err.append(errbuf, strlen(errbuf));
        err += "\n\n";
    }

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    file.close();

    return res;
}

int http_client::put(std::string url, std::string data, header_map headers = header_map())
{
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        err = "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // opening file and getting size
    std::stringstream s (data);
    curl_off_t fsz = data.size();

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url);
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, read);
    curl_easy_setopt(hdl, CURLOPT_READDATA, &s);
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, fsz);

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
    char errbuf[CURL_ERROR_SIZE];
    errbuf[0] = 0;
    curl_easy_setopt(hdl, CURLOPT_ERRORBUFFER, errbuf);
    int res = curl_easy_perform(hdl);
    if (strlen(errbuf))
    {
        err.append(errbuf, strlen(errbuf));
        err += "\n\n";
    }

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return res;
}

int http_client::put_f(std::string url, std::string filename, header_map headers = header_map())
{
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        err = "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // opening file and getting size
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        err += "Could not open file\n\n";
        return CURL_FILE_ERR;
    }
    struct stat finfo;
    stat(filename.c_str(), &finfo);
    curl_off_t fsz = finfo.st_size;

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url);
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, readf);
    curl_easy_setopt(hdl, CURLOPT_READDATA, &file);
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, fsz);

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
    char errbuf[CURL_ERROR_SIZE];
    errbuf[0] = 0;
    curl_easy_setopt(hdl, CURLOPT_ERRORBUFFER, errbuf);
    int res = curl_easy_perform(hdl);
    if (strlen(errbuf))
    {
        err.append(errbuf, strlen(errbuf));
        err += "\n\n";
    }

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return res;
}