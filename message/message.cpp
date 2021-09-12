#include <vector>

#include "message.h"

namespace message_ {

void to_bson(bson_t *b, const message_ptr msg) {
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

void to_bsonbuf(std::vector<uint8_t> &dst, const message_ptr msg) {
    dst.clear();

    bson_t b;
    bson_init(&b);

    to_bson(&b, msg);

    dst.resize(b.len);
    memcpy(dst.data(), bson_get_data(&b), dst.size());
}

void from_bson() {}
void from_bsonbuf() {}

}