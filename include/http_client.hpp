#ifndef __HTTP_CLIENT_HPP__
#define __HTTP_CLIENT_HPP__

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <curl/curl.h>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef header_map
#define header_map std::map<std::string, std::string>
#endif

#define CURL_BAD_HANDLE -1
#define CURL_FILE_ERR -2

#define MIME_STRING 0
#define MIME_FILE 1

class mime_part
{
    header_map hds;
    bool type;
public:
    mime_part(int t, header_map headers) : type(t), hds(headers) {}
    bool dtype() { return type; }
    const header_map& get_headers() { return hds; }
};

class mime_string_part : public mime_part
{
    std::string f;
    std::string d;
public:
    mime_string_part(std::string field, std::string data, header_map headers = header_map()) : mime_part(MIME_STRING, headers), f(field), d(data) {}
    const std::string& get_field() const { return f; }
    const std::string& get_data() const { return d; }
};

class mime_file_part : public mime_part
{
    std::string f;
    std::string pth;
    std::string rmt;
public:
    mime_file_part (std::string field, std::string path, std::string remotename, header_map headers = header_map()) : mime_part(MIME_FILE, headers), f(field), pth(path), rmt(remotename) {}
    const std::string& get_field() const { return f; }
    const std::string& get_path() const { return pth; }
    const std::string& get_remote() const { return rmt; }
};

class http_client
{
private:
    std::string err;
    bool log_en = false;

    static size_t write(void *buffer, size_t size, size_t nmemb, std::string* userp);
    static size_t writef(void *buffer, size_t size, size_t nmemb, std::ofstream* userp);
    static size_t read(void *buffer, size_t size, size_t nmemb, std::stringstream* userp);
    static size_t readf(void *buffer, size_t size, size_t nmemb, FILE* userp);
    curl_slist* bna_hds(CURL* hdl, header_map headers); // build and attach headers

public:

    int get(std::string url, long& rescode, std::string* response, header_map headers);
    int getfile(std::string url, long &rescode,std::string filename, header_map headers);
    int put(std::string url, long &rescode, std::string data, std::string* response, header_map headers);
    int putfile(std::string url, long &rescode, std::string filename, std::string* response, header_map headers);
    int simplepost(std::string url, long &rescode, std::string data, std::string * response, header_map headers);
    int binarypost(std::string url, long &rescode, void* data, long int size, std::string* response, header_map headers);
    int formpost(std::string url, long &rescode, std::vector<mime_part*> parts, std::string *response, header_map headers);

    int c_get(std::string type, std::string url, long &rescode,std::string* response, header_map headers);
    int c_getfile(std::string type, std::string url, long &rescode,std::string filename, header_map headers);
    int c_put(std::string type, std::string url, long &rescode, std::string data, std::string* response, header_map headers);
    int c_putfile(std::string type, std::string url, long &rescode, std::string filename, std::string* response, header_map headers);
    int c_simplepost(std::string type, std::string url, long &rescode, std::string data, std::string * response, header_map headers);
    int c_binarypost(std::string type, std::string url, long &rescode, void* data, long int size, std::string* response, header_map headers);
    int c_formpost(std::string type, std::string url, long &rescode, std::vector<mime_part*> parts, std::string *response, header_map headers);

    void enable_logging();
    void disable_logging();
    const char* log_status();
    std::string log();
    void free_log();
};