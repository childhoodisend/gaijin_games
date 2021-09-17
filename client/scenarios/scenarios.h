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

static const int64_t MSG_SIZE = 1000;
const std::vector<std::string> possible_commands{"get", "set"};
const std::vector<std::string> possible_values{"a", "b", "c", "d", "e", "f", "g"};

// get - OK
// set - errors : key already exist
std::vector<request_message_ptr> scenario_test_get_set_errors() {
    std::vector<request_message_ptr> msg_vec_ptr{};
    msg_vec_ptr.reserve(MSG_SIZE);

    for (size_t i = 0; i < MSG_SIZE; ++i) {
        std::string key         = "ddxi" + std::to_string(i) + "id";
        std::string value       = possible_values[i % possible_values.size()] + std::to_string(i);
        std::string cur_command = possible_commands[i % possible_commands.size()];
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

// server errors : key already exist
std::vector<request_message_ptr> scenario_test_set_errors() {
    std::vector<request_message_ptr> msg_vec_ptr{};
    msg_vec_ptr.reserve(MSG_SIZE);

    std::string cur_command = "set";

    for (size_t i = 0; i < MSG_SIZE; ++i) {
        if(i % 100 == 0) {
            std::string key = "ddxi" + std::to_string(i) + "id";
            std::string value = possible_values[i % possible_values.size()] + std::to_string(i);

            request_message_ptr ptr_t = std::make_shared<request_message>(cur_command, key, value);

            msg_vec_ptr.push_back(ptr_t);
        }
    }

    return msg_vec_ptr;
}

// get - OK
std::vector<request_message_ptr> scenario_test_get() {
    std::vector<request_message_ptr> msg_vec_ptr{};
    msg_vec_ptr.reserve(MSG_SIZE);

    for (size_t i = 0; i < MSG_SIZE; ++i) {
        std::string key         = "ddxi" + std::to_string(i) + "id";
        std::string cur_command = "get";

        request_message_ptr ptr_t = std::make_shared<request_message>(cur_command, key, "none");

        msg_vec_ptr.push_back(ptr_t);
    }

    return msg_vec_ptr;
}

// get errors :
std::vector<request_message_ptr> scenario_test_get_errors () {
    std::vector<request_message_ptr> msg_vec_ptr{};

    std::string bad_key     = "ddxi____id";
    std::string cur_command = "get";
    request_message_ptr ptr_t = std::make_shared<request_message>(cur_command, bad_key, "none");

    msg_vec_ptr.push_back(ptr_t);

    return msg_vec_ptr;
}

std::vector<request_message_ptr> scenario_test_set_unique() {
    std::vector<request_message_ptr> msg_vec_ptr{};
    msg_vec_ptr.reserve(MSG_SIZE);

    std::string cur_command = "set";

    for (size_t i = 0; i < MSG_SIZE; ++i) {
        if(i % 100 == 0) {
            std::string key = "ddxi" + std::to_string(i) + "uid";
            std::string value = possible_values[i % possible_values.size()] + std::to_string(i);

            request_message_ptr ptr_t = std::make_shared<request_message>(cur_command, key, value);

            msg_vec_ptr.push_back(ptr_t);
        }
    }

    return msg_vec_ptr;
}

#endif //GAIJIN_GAMES_SCENARIOS_H
