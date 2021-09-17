#ifndef GAIJIN_GAMES_DATA_WRITER_H
#define GAIJIN_GAMES_DATA_WRITER_H

#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <fstream>


namespace writer {

class Writer {
public:
    Writer();
    ~Writer();
    void add(const std::string key, const std::string value);

private:
    void run();
    void write_data();

private:
    std::ofstream fout;
    bool is_running = false;
    std::thread write_thread;
    std::mutex m;
    std::queue<std::pair<std::string, std::string>> q{};
};

typedef std::shared_ptr<Writer> writer_ptr;

}
#endif //GAIJIN_GAMES_DATA_WRITER_H
