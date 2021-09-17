#include <iostream>
#include <exception>
#include <vector>

#include "bson/bson.h"

#include "server.h"

Server::Server(uint16 port, const char *host) {
    std::cout << "Starting listening on port " << port << " host " << host << std::endl;
    is_running = true;

    listen_socket.Initialize();
    auto res = listen_socket.Listen(host, port);
    if (!res) {
        throw std::runtime_error(std::string("Server::Server() : Listen failed ")
                                 + listen_socket.GetServerAddr() + ":"
                                 + std::to_string(listen_socket.GetServerPort()) + " "
                                 + listen_socket.DescribeError());
    }

    listen_thread = std::thread(&Server::listen_run, this);
}

Server::~Server() {
    std::cout << "Server::~Server() " << std::endl;

    is_running = false;
    listen_socket.Shutdown(CSimpleSocket::Both);
    listen_socket.Close();

    if(listen_thread.joinable()) {
        listen_thread.join();
    }

    std::lock_guard<std::mutex> locker(active_sockets_mutex);
    for (const auto& ptr : active_sockets) {
        ptr->Shutdown(CSimpleSocket::Both);
        ptr->Close();
    }

}


void Server::listen_run() {
    while (is_running) {
        std::cout << "Server::listen_run() : wait for inbound connection on "
                                  << listen_socket.GetServerAddr() << ":"
                                  << listen_socket.GetServerPort() << std::endl;

        auto socket_ptr = listen_socket.Accept();
        if (socket_ptr) {
            on_accept(std::shared_ptr<CActiveSocket>(socket_ptr));
        } else {
            std::cerr << "Server::listen_run() : Accept() returned nullptr " << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(10));

    }
    std::cout << "Server::listen_run() FINISH " << std::endl;
}

void Server::on_accept(std::shared_ptr<CActiveSocket> socket_ptr) {
    std::cout << "Server::on_accept() " << describe_client(socket_ptr) << std::endl;

    if (!socket_ptr->SetBlocking()) {
        socket_ptr->Shutdown(CSimpleSocket::Both);
        socket_ptr->Close();

        throw std::runtime_error("Server::on_accept() exc : unimplemented set_blocking ");
    }

    auto receive_thr = std::thread(&Server::receive_run, this, socket_ptr);
    receive_thr.detach();

    std::lock_guard<std::mutex> locker(active_sockets_mutex);
    active_sockets.insert(socket_ptr);
}

std::string Server::describe_client(std::shared_ptr<CActiveSocket> socket_ptr) {
    std::string str;

    str += "Client = ";
    str += socket_ptr->GetClientAddr();
    str += ":";
    str += std::to_string(socket_ptr->GetClientPort());

    return str;
}

void Server::receive_run(std::shared_ptr<CActiveSocket> socket_ptr) {

    std::cout << "Server::receive_run() " << std::endl;

    try {
        while(is_running) {
            size_t size = 0;

            auto res1 = socket_ptr->Receive(sizeof (size_t), (uint8_t*)(&size));

            if(size == 0) {
                std::cerr << "Server::receive_run() exit " << std::endl;
                destroy_client(socket_ptr);
                break;
            }

            if (res1 == 0) {
                std::cout << "Server::receive_run(): disconnect err " << socket_ptr->GetSocketDescriptor() << std::endl;
            }
            if (res1 < 0) {
                std::cout << "Server::receive_run(): socket err " << socket_ptr->GetSocketDescriptor() << std::endl;
            }

            if(size != 0) {
                auto res2 = socket_ptr->Receive((int32_t)size);
                if (res2 == 0) {
                    std::cout << "Server::receive_run(): disconnect err " << socket_ptr->GetSocketDescriptor() << std::endl;
                }
                if (res2 < 0) {
                    std::cout << "Server::receive_run(): socket err " << socket_ptr->GetSocketDescriptor() << std::endl;
                }
                if (res2 != (int32_t)size) {
                    std::cout << "Server::receive_run(): err " << socket_ptr->GetSocketDescriptor()
                              << " receive " << res2 << " of " << size << std::endl;
                }

                bson_t b;
                bson_init_static(&b, socket_ptr->GetData(), size);

                message_::request_message_ptr cur_msg = std::make_shared<message_::request_message>();
                message_::from_bson(cur_msg, &b);
                bson_destroy(&b);

                handle_msg(cur_msg, socket_ptr);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    catch (std::exception &ex) {
        std::cerr << "Server::receive_run() exc : " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Server::receive_run() exc : ... " << std::endl;
    }
}

void Server::handle_msg(const message_::request_message_ptr& msg_ptr, std::shared_ptr<CActiveSocket> socket_ptr) {
    if (std::find(possible_commands.begin(), possible_commands.end(), msg_ptr->command) != possible_commands.end()) {
        if (msg_ptr->command == "get") {
            try {
                std::string val = get(msg_ptr->key);
                if(val == "none") {
                    return;
                }
                else {
                    message_::answer_message_ptr new_msg = std::make_shared<message_::answer_message>(msg_ptr->key, val);
                    send_msg_to_client(new_msg, socket_ptr);
                }
            }
            catch (std::runtime_error &rt) {
                std::cerr << "Server::handle_msg() err : " << rt.what() << std::endl;
                return;
            }
        }
        else if (msg_ptr->command == "set") {
            try {
                set(msg_ptr->key, msg_ptr->value);
            }
            catch (std::runtime_error &rt) {
                std::cerr << "Server::handle_msg() err : " << rt.what() << std::endl;
                return;
            }
        }
    }
    else {
        throw std::runtime_error("Server::handle_command() exc : Invalid command " + msg_ptr->command);
    }
}

void Server::send_msg_to_client(const message_::answer_message_ptr &msg_ptr, std::shared_ptr<CActiveSocket>& socket_ptr) {

    std::cout << "Server sent to " << describe_client(socket_ptr) << " --> " << std::endl;
    msg_ptr->print();

    std::vector<uint8_t> buffer{};
    message_::to_bsonbuf(buffer, msg_ptr);

    try {
        size_t size = buffer.size();
        auto res1 = socket_ptr->Send((uint8_t*)(&size), sizeof (size_t));
        auto res2 = socket_ptr->Send(buffer.data(), size);
        if (res1 == 0 || res2 == 0) {
            std::cout << "Server::send_msg_to_client() : disconnect err " << socket_ptr->GetSocketDescriptor() << std::endl;
        }
        if (res2 < 0 || res1 < 0) {
            std::cout << "Server::send_msg_to_client() : socket err " << socket_ptr->GetSocketDescriptor() << std::endl;
        }
        if (res2 != (int32_t)buffer.size()) {
            std::cout << "Server::send_msg_to_client(): err " << socket_ptr->GetSocketDescriptor()
                      << "sent " << res2 << " of " << size << std::endl;
        }
    }
    catch (...) {
        std::cerr << "Server::send_msg_to_client() exc : ... " << std::endl;
    }
}

std::string Server::get(const std::string &key) {

    //std::cout << "Server::get() " << std::endl;

    std::lock_guard<std::mutex> locker(data_mutex);
    if (data.find(key) != data.end()) {
        std::string value = data[key];
        return value;
    }
    else {
        throw std::runtime_error("Server::get() err : invalid key " + key);
    }
}

void Server::set(const std::string &key, const std::string &value) {

    //std::cout << "Server::set() " << std::endl;

    std::lock_guard<std::mutex> locker(data_mutex);
    if(data.find(key) != data.end()) {
        data.insert(std::make_pair(key, value));
        data_writer_ptr->add(key, value);
    }
    else {
        throw std::runtime_error("Server::set() err : key already exist " + key);
    }
}

void Server::destroy_client(std::shared_ptr<CActiveSocket> socket_ptr) {

    std::cout << "Server::destroy_client() " << describe_client(socket_ptr) << std::endl;

    std::lock_guard<std::mutex> locker(active_sockets_mutex);

    socket_ptr->Shutdown(CSimpleSocket::Both);
    socket_ptr->Close();

    active_sockets.erase(socket_ptr);

    std::cout << "Clients left " << active_sockets.size() << std::endl;

}

int main() {
    try {
        auto server = std::make_shared<Server>(4000, "127.0.0.1");
        while(server->is_running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    catch (std::exception &ex) {
        std::cerr << "int main() exc : " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "int main() exc : ... " << std::endl;
    }

    return EXIT_SUCCESS;
}
