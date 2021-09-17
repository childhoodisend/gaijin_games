#include "data_writer.h"
#include "fstream"

namespace writer {

Writer::Writer() {
    is_running = true;
    write_thread = std::thread(&Writer::run, this);
    fout.open("../../server/config.txt", std::ios_base::out | std::ios_base::app);
    fout << std::endl;
}
Writer::~Writer(){
    if(write_thread.joinable()) {
        write_thread.join();
    }

    is_running = false;
    fout.close();
}


void Writer::run() {
    while(true) {
        if (q.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        else {
            write_data();
        }
    }
}

void Writer::add(const std::string key, const std::string value) {
    std::lock_guard<std::mutex> locker(m);
    q.emplace(std::make_pair(key, value));
}

void Writer::write_data() {
    std::lock_guard<std::mutex> locker(m);
    while(!q.empty()) {
        auto msg = q.front();
        fout << msg.first << " " << msg.second << std::endl;
        q.pop();
    }
}

}