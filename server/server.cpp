#include <iostream>
#include <exception>
#include <vector>

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
    listen_thread.join();
}

void Server::listen_run() {
    while (is_running) {
        std::cout << "Server::listen_run() : wait for inbound connection on "
                  << listen_socket.GetServerAddr() << ":" << listen_socket.GetServerPort() << std::endl;

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

void Server::receive_run(std::shared_ptr<CActiveSocket>) {
    try {
        std::vector<uint8_t> buffer;

    }
    catch (...) {

    }
}

int main() {
    try {
        auto server = std::make_shared<Server>(4000, "127.0.0.1");
    }
    catch (std::exception &ex) {
        std::cerr << "Catched server exc : " << ex.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Another exc : " << std::endl;
    }

    return EXIT_SUCCESS;
}
