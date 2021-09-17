#ifndef GAIJIN_GAMES_STATISTER_H
#define GAIJIN_GAMES_STATISTER_H

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <unordered_map>

namespace statister {

class Statister {
public:
    Statister();
    ~Statister();

    void reads_key(const std::string& key);
    void writes_key(const std::string& key);
    int64_t get_reads(const std::string& key);
    int64_t get_writes(const std::string& key);
private:
    void run();
    void print_statistic();
    void clear_last_statisctic();

    void reads_inc();
    void writes_inc();

private:
    bool is_running = false;
    std::thread statister_thread;

    std::mutex m;

    int64_t writes_all       = 0;
    int64_t reads_all        = 0;
    int64_t writes_last5_sec = 0;
    int64_t reads_last_5sec  = 0;

    std::unordered_map<std::string, std::pair<int64_t, int64_t>> key_stat{}; // first - writes, second - reads
};

typedef std::shared_ptr<statister::Statister> statister_ptr;

}

#endif //GAIJIN_GAMES_STATISTER_H
