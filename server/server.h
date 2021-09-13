#include <thread>
#include <mutex>
#include <string>
#include <unordered_set>
#include <map>
#include <memory>

#include "clsocket/src/ActiveSocket.h"
#include "clsocket/src/PassiveSocket.h"

#include "message/message.h"

#ifndef GAIJIN_GAMES_SERVER_H
#define GAIJIN_GAMES_SERVER_H


class Server {
public:
    Server(uint16 port, const char* host);
    ~Server();
private:
    std::string get(const std::string& key);
    void set(const std::string& key, const std::string& value);

    void handle_msg(const message_::request_message_ptr& msg_ptr, std::shared_ptr<CActiveSocket> socket_ptr);
    void send_msg_to_client(const message_::answer_message_ptr& msg_ptr, std::shared_ptr<CActiveSocket>& socket_ptr);

    void listen_run();
    void receive_run(std::shared_ptr<CActiveSocket> socket_ptr);
    void on_accept(std::shared_ptr<CActiveSocket> socket_ptr);
    std::string describe_client(std::shared_ptr<CActiveSocket> socket_ptr);

public:
    bool is_running = false;
private:
    std::vector<std::string> possible_commands = {"get", "set"};

    std::thread listen_thread;
    std::thread logger_thread;

    std::mutex active_sockets_mutex;
    CPassiveSocket listen_socket;
    std::unordered_set<std::shared_ptr<CActiveSocket>> active_sockets;

    std::mutex file_mutex;
    std::string file = "config.txt";

    std::map<std::string, std::string> data{};
};


#endif //GAIJIN_GAMES_SERVER_H
