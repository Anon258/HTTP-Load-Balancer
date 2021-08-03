#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <time.h>

class logger
{
    std::string filename;
    std::ofstream fd;
    std::queue<std::string> outbuff;
    std::mutex buffMutex;
    std::thread t;
    std::condition_variable buffHasData;

    void log()
    {
        while (true)
        {
            std::unique_lock<std::mutex> bufflk(buffMutex);
            buffHasData.wait(bufflk, [this]{ return !outbuff.empty();});
            std::string tolog = outbuff.front();
            outbuff.pop();
            bufflk.unlock();
            fd.open(filename, std::ios::app);
            fd << tolog << "\n";
            fd.close();
        }
    }

public:
    logger(std::string file) : filename(file), t(&logger::log, this)
    {
        fd.open(filename, std::ios::app);
        time_t today;
        time(&today);
        fd << "STARTING LOGGING AT " << asctime(localtime(&today)) << "\n\n";
        fd.close();
    }

    void add_log(const std::string& s)
    {
        std::lock_guard<std::mutex> bufflk(buffMutex);
        outbuff.push(s);
        buffHasData.notify_one();
    }
};

#endif