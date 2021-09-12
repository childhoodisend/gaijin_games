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

    for (const auto& scena : scenario) {
        std::vector<uint8_t> buffer;
        for (const auto& msg : scena.msg) {
            message_::to_bsonbuf(buffer, msg);

            socket_ptr->Send(buffer.data(), buffer.size());

            std::this_thread::sleep_for(std::chrono::microseconds(scena.pause));
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    socket_ptr->Shutdown(CSimpleSocket::Both);
    socket_ptr->Close();
}

int main () {
    scenario_t scenario = {{simple_scenario(), 10}};

    std::vector<std::thread> clients{};
    clients.emplace_back(&client_run, scenario);

    for (auto& client : clients) {
        if(client.joinable()) {
            client.join();
        }
    }

    return EXIT_SUCCESS;
}