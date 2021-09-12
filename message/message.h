#ifndef GAIJIN_GAMES_MESSAGE_H
#define GAIJIN_GAMES_MESSAGE_H

#pragma once

#include <string>
#include <memory>
#include <utility>

#include <bson/bson.h>

namespace message_ {
struct message {
public:
    message(std::string _com, std::string _key, std::string _value) : command(std::move(_com)),
                                                                      key(std::move(_key)),
                                                                      value(std::move(_value)) {};

    ~message() = default;

public:
    std::string command{};
    std::string key{};
    std::string value{};
};

typedef std::shared_ptr<message> message_ptr;

void print(const message_ptr& msg);
void to_bson(bson_t *b, const message_ptr& msg);
void to_bsonbuf(std::vector<uint8_t> &dst, const message_ptr& msg);
void from_bson();
void from_bsonbuf();

}
#endif //GAIJIN_GAMES_MESSAGE_H
