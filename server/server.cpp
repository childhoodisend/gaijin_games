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
template<typename T>
void print(std::vector<T>& vec) {
    for(const auto& el : vec) {
        std::cout << el << " ";
    }
    std::cout << std::endl;
}
void Server::receive_run(std::shared_ptr<CActiveSocket> socket_ptr) {

    std::cout << "Server::receive_run() " << std::endl;

    try {
        size_t sleep_counter = 0;
        while(is_running) {
            size_t size = 0;

            // TODO add checks
            socket_ptr->Receive(sizeof (size_t), (uint8_t*)(&size));
            if(size != 0) {
                socket_ptr->Receive((int32_t)size);

//                handle_msg();
                bson_t b;
                bson_init_static(&b, socket_ptr->GetData(), size);
                char *j = bson_as_canonical_extended_json(&b, nullptr);
                std::cout << describe_client(socket_ptr) << " received msgs : " << j << std::endl;
                bson_destroy(&b);
                bson_free(j);
            }
            if(size == 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                ++sleep_counter;
                std::cout << "Sleep " << sleep_counter << " times "<< std::endl;
            }
            if(sleep_counter == 10) {
                std::cout << "Slept " << sleep_counter * 1000 << " ms" << std::endl;
                std::cout << "Disconnect " << describe_client(socket_ptr) << std::endl;
                socket_ptr->Shutdown(CSimpleSocket::Sends);
                socket_ptr->Close();
                break;
            }
        }
    }
    catch (std::exception &ex) {
        std::cerr << "Server::receive_run() exc : " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Server::receive_run() exc : ... " << std::endl;
    }
}

//void Server::handle_msg(const message_::message_ptr& msg_ptr) {
//    if (std::find(possible_commands.begin(), possible_commands.end(), msg_ptr->command) != possible_commands.end()) {
//        if (msg_ptr->command == "get") {
//            std::string got_val = get(msg_ptr->key);
////            send_msg_to_client(got_val);
//        }
//    }
//    else {
//        throw std::runtime_error("Server::handle_command() exc : Invalid command " + msg_ptr->command);
//    }
//}

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
