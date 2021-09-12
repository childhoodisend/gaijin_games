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
        while(true) {
            std::vector<uint8_t> buffer{};
            auto res = socket_ptr->Receive(); // TODO add checks
            if(res) {
                std::cout << socket_ptr->GetData() << std::endl;
//                memcpy(buffer.data(), socket_ptr->GetData(), socket_ptr->GetBytesReceived());
//                std::cout << "size " << buffer.size() << std::endl;
                buffer.clear();
            }
//        bson_t b;
//        bson_init_static(&b, buffer.data(), buffer.size() * sizeof (uint8_t));
//        char *j = bson_as_canonical_extended_json(&b, nullptr);
//        std::cout << describe_client(socket_ptr) << " received msg : " << j << std::endl;
//        bson_destroy(&b);
//        bson_free(j);
        }
    }
    catch (std::exception &ex) {
        std::cerr << "Server::receive_run() exc : " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Server::receive_run() exc : ... " << std::endl;
    }
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
