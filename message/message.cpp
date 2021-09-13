#include <vector>
#include <iostream>

#include "message.h"
#include "bson_conv.h"

namespace message_ {

void to_bson(bson_t *b, const message_ptr& msg) {

    if (!BSON_APPEND_UTF8(b, "command", msg->command.c_str())) {
        throw std::runtime_error("to_bson() err : can't append command ");
    }
    if (!BSON_APPEND_UTF8(b, "key", msg->key.c_str())) {
        throw std::runtime_error("to_bson() err : can't append key ");
    }
    if (!BSON_APPEND_UTF8(b, "value", msg->value.c_str())) {
        throw std::runtime_error("to_bson() err : can't append value ");
    }
}

void from_bson(message_ptr& msg_ptr, const bson_t* b) {
    msg_ptr->command = bson_conv::get_simple_field<std::string>(b, "command", true);
    msg_ptr->key     = bson_conv::get_simple_field<std::string>(b, "key"    , true);
    msg_ptr->value   = bson_conv::get_simple_field<std::string>(b, "value"  , true);
}

void from_bsonbuf(message_ptr& t, const std::vector<uint8_t> &buffer) {
    bson_t b;

    if(!bson_init_static(&b, buffer.data(), buffer.size())) {
        throw std::runtime_error("from_bsonbuf() bson init static failed ");
    }
    try {
        from_bson(t, &b);
    }
    catch (std::runtime_error &rt) {
        std::cout << "from_bsonbuf() exc : " << rt.what() << std::endl;
    }
}

void to_bsonbuf(std::vector<uint8_t> &dst, const message_ptr& t) {

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


void print(const message_ptr& msg) {
    std::cout << "\"command\" : " << "\"" << msg->command << "\"" << std::endl;
    std::cout << "\"key\"     : " << "\"" << msg->key     << "\""<< std::endl;
    std::cout << "\"value\"   : " << "\"" << msg->value   << "\""<< std::endl;
}
}