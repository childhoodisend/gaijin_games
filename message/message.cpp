#include <vector>
#include <iostream>

#include "message.h"

namespace message_ {

void to_bson(bson_t *b, const message_ptr& msg) {

    //std::cout << "to_bson() " << std::endl;

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

void to_bsonbuf(std::vector<uint8_t> &dst, const message_ptr& msg) {

    //std::cout << "to_bsonbuf() " << std::endl;

    dst.clear();

    bson_t b;
    bson_init(&b);
    try {
        to_bson(&b, msg);
    }
    catch (std::runtime_error &rt) {
        std::cerr << "to_bsonbuf() exc : " << rt.what() << std::endl;
    }
    dst.resize(b.len);
    char* j = bson_as_canonical_extended_json(&b, nullptr);
    std::cout << j << std::endl;
    bson_free(j);
    memcpy(dst.data(), bson_get_data(&b), dst.size());
}

void from_bson() {}
void from_bsonbuf() {}

void print(const message_ptr& msg) {
    std::cout << "command " << msg->command << std::endl;
    std::cout << "key "     << msg->key << std::endl;
    std::cout << "value "   << msg->value << std::endl;
}
}