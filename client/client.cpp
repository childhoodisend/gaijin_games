#include <thread>
#include <vector>
#include <iostream>

#include "scenarios/scenarios.h"
#include "message/message.h"

#include "../clsocket/src/ActiveSocket.h"


struct client_config {
    std::string server_host = "127.0.0.1";
    int16 port = 4000;
} cfg;

void client_run(scenario_t scenario){

    auto socket_ptr = std::make_shared<CActiveSocket>();
    socket_ptr->Initialize();

    if(!socket_ptr->Open(cfg.server_host.c_str(), cfg.port)) {
        std::cerr << "client_run() can't connect " << std::endl;
        return;
    }
    socket_ptr->SetBlocking();

    std::thread read_thread([&]{
        //receive data
    });

    for (const auto& scena : scenario) {
        std::vector<uint8_t> buffer{};
        for (const auto& msg : scena.msgs) {
            std::cout << "Sent --> " << std::endl;
            msg->print();
            std::cout << std::endl;

            message_::to_bsonbuf(buffer, msg);

            try {
                size_t size = buffer.size();
                auto res1 = socket_ptr->Send((uint8_t*)(&size), sizeof (size_t));
                auto res2 = socket_ptr->Send(buffer.data(), size);
                if (res1 == 0 || res2 == 0) {
                    std::cout << "send_thread: disconnect err " << socket_ptr->GetSocketDescriptor() << std::endl;
                }
                if (res2 < 0 || res1 < 0) {
                    std::cout << "send_thread: socket err " << socket_ptr->GetSocketDescriptor() << std::endl;
                }
                if (res2 != (int32_t)buffer.size()) {
                    std::cout << "send_thread: err " << socket_ptr->GetSocketDescriptor()
                    << "sent " << res2 << " of " << size << std::endl;
                }
            }
            catch (...) {
                std::cerr << "client_run() exc : ... " << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(scena.pause));
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    socket_ptr->Shutdown(CSimpleSocket::Both);
    socket_ptr->Close();

    read_thread.join();
}

int main () {
    scenario_t scenario = {{simple_scenario(), 1000}};

    std::vector<std::thread> clients{};
    clients.emplace_back(&client_run, scenario);

    for (auto& client : clients) {
        if(client.joinable()) {
            client.join();
        }
    }

    return EXIT_SUCCESS;
}