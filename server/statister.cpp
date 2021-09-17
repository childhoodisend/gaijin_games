#include <iostream>

#include "statister.h"

namespace statister {
Statister::Statister() {
    is_running = true;

    statister_thread = std::thread(&Statister::run, this);

}

Statister::~Statister() {
    is_running = false;

    if(statister_thread.joinable()) {
        statister_thread.join();
    }

}

void Statister::run() {
    while(is_running) {
        print_statistic();

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        clear_last_statisctic();
    }
}

void Statister::print_statistic() {
    std::cout << "Statister         " << std::endl;
    std::cout << "ALL READS         " << reads_all        << std::endl;
    std::cout << "ALL WRITES        " << writes_all       << std::endl;
    std::cout << "READS  LAST 5 SEC " << reads_last_5sec  << std::endl;
    std::cout << "WRITES LAST 5 SEC " << writes_last5_sec << std::endl;
    std::cout << std::endl;

}

void Statister::clear_last_statisctic() {
    std::lock_guard<std::mutex> locker(m);
    writes_last5_sec = 0;
    reads_last_5sec  = 0;
}

void Statister::reads_inc() {
    std::lock_guard<std::mutex> locker(m);
    reads_all++;
    reads_last_5sec++;
}
void Statister::reads_inc_unsafe() {
    reads_all++;
    reads_last_5sec++;
}

void Statister::writes_inc() {
    std::lock_guard<std::mutex> locker(m);
    writes_all++;
    writes_last5_sec++;
}
void Statister::writes_inc_unsafe() {
    writes_all++;
    writes_last5_sec++;
}

void Statister::reads_key(const std::string &key) {
    std::lock_guard<std::mutex> locker(m);
    auto it = key_stat.find(key);
    if (it != key_stat.end()) {
        it->second.second++;
        reads_inc_unsafe();
    }
    else {
        std::cerr << "Statister::reads_key() __warning__ : new key " << key << std::endl;
        key_stat[key] = {0, 1};
    }
}

void Statister::writes_key(const std::string &key) {
    std::lock_guard<std::mutex> locker(m);
    auto it = key_stat.find(key);
    if (it != key_stat.end()) {
        it->second.first++;
        writes_inc_unsafe();
    }
    else {
        std::cerr << "Statister::writes_key() __warning__ : new key " << key << std::endl;
        key_stat[key] = {1, 0};
    }
}

}