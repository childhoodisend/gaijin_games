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

void from_bson(message_ptr& , const bson_t* );
void to_bson(bson_t *b, const message_ptr& msg);

void from_bsonbuf(message_ptr& t, const std::vector<uint8_t> &buffer);

void to_bsonbuf(std::vector<uint8_t> &dst, const message_ptr& t);


void print(const message_ptr& msg);

}
#endif //GAIJIN_GAMES_MESSAGE_H
