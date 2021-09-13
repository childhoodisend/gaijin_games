#ifndef GAIJIN_GAMES_MESSAGE_H
#define GAIJIN_GAMES_MESSAGE_H

#pragma once

#include <string>
#include <memory>
#include <utility>
#include <vector>

#include <bson/bson.h>
#include "bson_conv.h"

namespace message_ {
struct request_message {
public:
    request_message() = default;
    request_message(const std::string& _com,const std::string& _key,const std::string& _value) : command(_com),
                                                                      key(_key),
                                                                      value(_value) {};

    ~request_message() = default;

    void print() const;

    std::string command{};
    std::string key{};
    std::string value{};
};

struct answer_message {
public:
    answer_message() = default;
    answer_message(const std::string& _key,const std::string& _value) : key(_key), value(_value) {};
    ~answer_message() = default;

    void print() const;

    std::string key{};
    std::string value{};
};

typedef std::shared_ptr<request_message> request_message_ptr;
typedef std::shared_ptr<answer_message>  answer_message_ptr;


template<typename T>
inline void to_bson(bson_t *b, const T& msg_ptr);

template<typename T>
inline void from_bson(T& msg_ptr, const bson_t* b);

template<>
inline void to_bson<request_message_ptr>(bson_t *b, const request_message_ptr& msg_ptr) {
    bson_conv::append_simple_field(b, "command", msg_ptr->command);
    bson_conv::append_simple_field(b, "key", msg_ptr->key);
    bson_conv::append_simple_field(b, "value", msg_ptr->value);
}
template<>
inline void from_bson<request_message_ptr>(request_message_ptr& msg_ptr, const bson_t* b) {
    bson_conv::get_simple_field<std::string>(msg_ptr->command, b, "command", true);
    bson_conv::get_simple_field<std::string>(msg_ptr->key, b, "key"    , true);
    bson_conv::get_simple_field<std::string>(msg_ptr->value, b, "value"  , true);
}

template<>
inline void to_bson<answer_message_ptr>(bson_t *b, const answer_message_ptr& msg_ptr){
    bson_conv::append_simple_field(b, "key", msg_ptr->key);
    bson_conv::append_simple_field(b, "value", msg_ptr->value);
}
template<>
inline void from_bson<answer_message_ptr>(answer_message_ptr& msg_ptr, const bson_t* b){
    bson_conv::get_simple_field<std::string>(msg_ptr->key, b, "key"    , true);
    bson_conv::get_simple_field<std::string>(msg_ptr->value, b, "value"  , true);
}



template<typename T>
void from_bsonbuf(T& t, const std::vector<uint8_t> &src){
    bson_t b;

    if(!bson_init_static(&b, src.data(), src.size())) {
        throw std::runtime_error("from_bsonbuf() bson init static failed ");
    }
    try {
        from_bson(t, &b);
    }
    catch (std::runtime_error &rt) {
        std::cout << "from_bsonbuf() exc : " << rt.what() << std::endl;
    }
}

template<typename T>
void to_bsonbuf(std::vector<uint8_t> &dst, const T& t){
    dst.clear();

    bson_t b;
    bson_init(&b);
    try {
        to_bson(&b, t);
    }
    catch (std::runtime_error &rt) {
        std::cerr << "to_bsonbuf() exc : " << rt.what() << std::endl;
    }
    dst.resize(b.len);
    memcpy(dst.data(), bson_get_data(&b), b.len);
}

}
#endif //GAIJIN_GAMES_MESSAGE_H
