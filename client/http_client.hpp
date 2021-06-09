#include <map>
#include <string>
#include <fstream>
#include <curl/curl.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
        userp->read((char*) buffer, size*nmemb);
        return userp->gcount();
    }
    static size_t readf(void *buffer, size_t size, size_t nmemb, FILE* userp)
    {
        int bytes_read = fread(buffer, size, nmemb, userp);
        return bytes_read;
    }
    curl_slist* bna_hds(CURL* hdl, header_map headers) // build and attach headers
    {
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
        return hds;
    }
public:
    int get(std::string url,std::string* response, header_map headers);
    int getfile(std::string url,std::string filename, header_map headers);
    int put(std::string url, std::string data, std::string* response, header_map headers);
    int putfile(std::string url, std::string filename, std::string* response, header_map headers);
    int custom(std::string url, std::string type, std::string body, std::string* response, header_map headers);
    int custom_tof(std::string url, std::string type, std::string body, std::string filename, header_map headers);
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
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    err += curl_easy_strerror(res);
    err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::getfile(std::string url, std::string filename, header_map headers = header_map())
{
    err += "getfile() :\n";
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
    if ( !file )
    {
        err += "Error in opening file\n\n";
        return CURL_FILE_ERR;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, writef);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, &file);
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    err += curl_easy_strerror(res);
    err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    file.close();

    return (int)res;
}

int http_client::put(std::string url, std::string data, std::string* response, header_map headers)
{
    err += "put()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // stringstream for readfunction
    std::stringstream ss(data);


    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, read);
    curl_easy_setopt(hdl, CURLOPT_READDATA, &ss);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, data.size());
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    err += curl_easy_strerror(res);
    err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::putfile(std::string url, std::string filename, std::string* response, header_map headers)
{
    err += "putfile()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // file handling for file to PUT
    FILE* file = fopen(filename.c_str(), "rb");
    struct stat fileinfo;
    stat(filename.c_str(), &fileinfo);
    curl_off_t filesz = fileinfo.st_size;

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, readf);
    curl_easy_setopt(hdl, CURLOPT_READDATA, file);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, filesz);
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    err += curl_easy_strerror(res);
    err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    fclose(file);

    return (int)res;
}

int http_client::custom(std::string url, std::string type, std::string body, std::string* response, header_map headers = header_map())
{
    err += "custom()" + type + " :\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // stringstream for readfunction
    std::stringstream ss(body);


    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, read);
    curl_easy_setopt(hdl, CURLOPT_READDATA, &ss);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, body.size());
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    err += curl_easy_strerror(res);
    err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::custom_tof(std::string url, std::string type, std::string body, std::string filename, header_map headers = header_map())
{
    err += "custom()" + type + " :\n";
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
    if ( !file )
    {
        err += "Error in opening file\n\n";
        return CURL_FILE_ERR;
    }

    // stringstream for readfunction
    std::stringstream ss(body);


    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, writef);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, read);
    curl_easy_setopt(hdl, CURLOPT_READDATA, &ss);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, body.size());
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    err += curl_easy_strerror(res);
    err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    file.close();

    return (int)res;
}