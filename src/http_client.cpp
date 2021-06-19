#include "http_client.hpp"

static size_t http_client::write(void *buffer, size_t size, size_t nmemb, std::string* userp)
{
    userp->append((cha::r*) buffer, size*nmemb);
    return size*nmemb;
}
static size_t http_client::writef(void *buffer, size_t size, size_t nmemb, std::ofstream* userp)
{
    userp->write((char*) buffer, size*nmemb);
    return size*nmemb;
}
static size_t http_client::read(void *buffer, size_t size, size_t nmemb, std::stringstream* userp)
{
    userp->read((char*) buffer, size*nmemb);
    return userp->gcount();
}
static size_t http_client::readf(void *buffer, size_t size, size_t nmemb, FILE* userp)
{
    int bytes_read = fread(buffer, size, nmemb, userp);
    return bytes_read;
}
curl_slist* http_client::bna_hds(CURL* hdl, header_map headers) // build and attach headers
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

int http_client::get(std::string url, long &rescode, std::string* response, header_map headers = header_map())
{
    if(log_en) err += "get() :\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::c_get(std::string type, std::string url, long &rescode, std::string* response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "get() :\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::getfile(std::string url, long &rescode, std::string filename, header_map headers = header_map())
{
    if(log_en) err += "getfile() :\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // opening file
    std::ofstream file;
    file.open(filename);
    if ( !file )
    {
        if(log_en) err += "Error in opening file\n\n";
        return CURL_FILE_ERR;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, writef);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, &file);
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    file.close();

    return (int)res;
}

int http_client::c_getfile(std::string type, std::string url, long &rescode, std::string filename, header_map headers = header_map())
{
    if(log_en) err += "getfile() :\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // opening file
    std::ofstream file;
    file.open(filename);
    if ( !file )
    {
        if(log_en) err += "Error in opening file\n\n";
        return CURL_FILE_ERR;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, writef);
    curl_easy_setopt(hdl, CURLOPT_WRITEDATA, &file);
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    file.close();

    return (int)res;
}

int http_client::put(std::string url, long &rescode, std::string data, std::string* response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "put()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // stringstream for readfunction
    std::stringstream ss(data);


    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, read);
    curl_easy_setopt(hdl, CURLOPT_READDATA, &ss);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, data.size());
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::c_put(std::string type, std::string url, long &rescode, std::string data, std::string* response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "put()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // stringstream for readfunction
    std::stringstream ss(data);


    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, read);
    curl_easy_setopt(hdl, CURLOPT_READDATA, &ss);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, data.size());
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::putfile(std::string url, long &rescode, std::string filename, std::string* response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "putfile()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // file handling for file to PUT
    FILE* file = fopen(filename.c_str(), "rb");
    struct stat fileinfo;
    stat(filename.c_str(), &fileinfo);
    curl_off_t filesz = fileinfo.st_size;

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {    
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, readf);
    curl_easy_setopt(hdl, CURLOPT_READDATA, file);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, filesz);
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    fclose(file);

    return (int)res;
}

int http_client::c_putfile(std::string type, std::string url, long &rescode, std::string filename, std::string* response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "putfile()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // file handling for file to PUT
    FILE* file = fopen(filename.c_str(), "rb");
    struct stat fileinfo;
    stat(filename.c_str(), &fileinfo);
    curl_off_t filesz = fileinfo.st_size;

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {    
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_READFUNCTION, readf);
    curl_easy_setopt(hdl, CURLOPT_READDATA, file);
    curl_easy_setopt(hdl, CURLOPT_INFILESIZE_LARGE, filesz);
    curl_easy_setopt(hdl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());
    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    fclose(file);

    return (int)res;
}

int http_client::simplepost(std::string url, long &rescode, std::string data, std::string * response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "simplepost()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {    
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_POSTFIELDS, data.c_str());

    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::c_simplepost(std::string type, std::string url, long &rescode, std::string data, std::string * response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "simplepost()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {    
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());

    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;
}

int http_client::binarypost(std::string url, long &rescode, void* data, long int size, std::string* response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "binarypost()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {    
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(hdl, CURLOPT_POSTFIELDSIZE, size);

    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;    
}

int http_client::c_binarypost(std::string type, std::string url, long &rescode, void* data, long int size, std::string* response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "binarypost()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {    
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }
    curl_easy_setopt(hdl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(hdl, CURLOPT_POSTFIELDSIZE, size);
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());

    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);

    return (int)res;    
}

int http_client::formpost(std::string url, long &rescode, std::vector<mime_part*> parts, std::string *response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "formpost()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {    
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }

    curl_mime* mpf = curl_mime_init(hdl);
    for (auto p : parts)
    {   
        curl_mimepart* part = curl_mime_addpart(mpf);
        
        curl_slist* d_hds = NULL;
        if (!p->get_headers().empty())
        {
            for(auto h : p->get_headers())
            {
                std::string header = h.first + ":" + h.second;
                d_hds = curl_slist_append(d_hds, header.c_str());
            }
            curl_mime_headers(part, d_hds, true);
        }

        if ( p->dtype() == MIME_STRING )
        {
            mime_string_part* cp = (mime_string_part*) p;
            curl_mime_name(part, cp->get_field().c_str());
            curl_mime_data(part, cp->get_data().c_str(), CURL_ZERO_TERMINATED);
        }
        else
        {
            mime_file_part* cp = (mime_file_part*) p;
            curl_mime_name(part, cp->get_field().c_str());
            curl_mime_filename(part, cp->get_remote().c_str());
            curl_mime_filedata(part, cp->get_path().c_str());
        }
    }

    curl_easy_setopt(hdl, CURLOPT_MIMEPOST, mpf);

    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    curl_mime_free(mpf);

    return (int)res;    
}

int http_client::c_formpost(std::string type, std::string url, long &rescode, std::vector<mime_part*> parts, std::string *response = nullptr, header_map headers = header_map())
{
    if(log_en) err += "formpost()\n";
    // handle initialization
    CURL* hdl = curl_easy_init();
    if (!hdl)
    {
        if(log_en) err += "Error in handle initialization\n\n";
        return CURL_BAD_HANDLE;
    }

    // option setting
    curl_easy_setopt(hdl, CURLOPT_URL, url.c_str());
    if (response)
    {    
        curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, write);
        curl_easy_setopt(hdl, CURLOPT_WRITEDATA, response);
    }

    curl_mime* mpf = curl_mime_init(hdl);
    for (auto p : parts)
    {   
        curl_mimepart* part = curl_mime_addpart(mpf);
        
        curl_slist* d_hds = NULL;
        if (!p->get_headers().empty())
        {
            for(auto h : p->get_headers())
            {
                std::string header = h.first + ":" + h.second;
                d_hds = curl_slist_append(d_hds, header.c_str());
            }
            curl_mime_headers(part, d_hds, true);
        }

        if ( p->dtype() == MIME_STRING )
        {
            mime_string_part* cp = (mime_string_part*) p;
            curl_mime_name(part, cp->get_field().c_str());
            curl_mime_data(part, cp->get_data().c_str(), CURL_ZERO_TERMINATED);
        }
        else
        {
            mime_file_part* cp = (mime_file_part*) p;
            curl_mime_name(part, cp->get_field().c_str());
            curl_mime_filename(part, cp->get_remote().c_str());
            curl_mime_filedata(part, cp->get_path().c_str());
        }
    }

    curl_easy_setopt(hdl, CURLOPT_MIMEPOST, mpf);
    curl_easy_setopt(hdl, CURLOPT_CUSTOMREQUEST, type.c_str());

    curl_slist* hds = bna_hds(hdl, headers);

    // perform
    CURLcode res = curl_easy_perform(hdl);
    curl_easy_getinfo(hdl, CURLINFO_RESPONSE_CODE, &rescode);
    if(log_en) err += curl_easy_strerror(res);
    if(log_en) err += "\n\n";

    // cleanup
    curl_slist_free_all(hds);
    curl_easy_cleanup(hdl);
    curl_mime_free(mpf);

    return (int)res;    
}

void http_client::enable_logging() { log_en = true; }
void http_client::disable_logging() { log_en = false; }
const char* http_client::log_status() { return log_en ? "enabled" : "disabled"; }
std::string http_client::log() { return err; }
void http_client::free_log() { err.clear(); }
