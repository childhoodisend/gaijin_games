#ifndef GAIJIN_GAMES_SCENARIOS_H
#define GAIJIN_GAMES_SCENARIOS_H

#pragma once

#include <vector>
#include <memory>
#include "message/message.h"

using namespace message_;

struct msgs_and_pause {
public:
    msgs_and_pause(std::vector<request_message_ptr> _msgs, int _pause) : msgs(std::move(_msgs)) , pause(_pause) {};

    std::vector<request_message_ptr> msgs{};
    int pause = 0;
};

typedef std::vector<msgs_and_pause> scenario_t;

static const int64_t MSG_SIZE = 100;
const std::vector<std::string> possible_commands{"get", "set"};
const std::vector<std::string> possible_values{"a", "b", "c", "d", "e", "f", "g"};

std::vector<request_message_ptr> simple_scenario() {
    std::vector<request_message_ptr> msg_vec_ptr{};
    msg_vec_ptr.reserve(MSG_SIZE);

    for (size_t i = 0; i < MSG_SIZE; ++i) {
        std::string key         = "ddxi" + std::to_string(i) + "id";
        std::string value       = possible_values[i % 7] + std::to_string(i);
        std::string cur_command = possible_commands[i % 2];
        request_message_ptr ptr_t = nullptr;

        if (cur_command == "set") {
            ptr_t = std::make_shared<request_message>(cur_command, key, value);
        }
        else if (cur_command == "get") {
            ptr_t = std::make_shared<request_message>(cur_command, key, "none");
        }
        else {
            throw std::runtime_error("simple_scenario() err : bad possible command " + cur_command);
        }

        msg_vec_ptr.push_back(ptr_t);
    }

    return msg_vec_ptr;
}


#endif //GAIJIN_GAMES_SCENARIOS_H
