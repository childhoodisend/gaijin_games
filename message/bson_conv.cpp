#include <iostream>
#include <exception>

#include "bson_conv.h"


namespace bson_conv {

template<typename T> inline bson_type_t get_bson_type() {
    throw std::runtime_error("get_bson_type() bad cast ");
}
template<> inline bson_type_t get_bson_type<std::string>() {return BSON_TYPE_UTF8; }

template<typename T> inline T get_bson_value(const bson_iter_t* it){
    throw std::runtime_error("get_bson_value() bad cast ");
}
template<> inline std::string get_bson_value<std::string>(const bson_iter_t* it) {
    uint32_t len;
    return {bson_iter_utf8(it, &len)};
}

template<typename T>
T get_simple_field(const bson_t* b, const std::string& key, bool is_required) {
    bson_type_t bson_type = get_bson_type<T>();
    bson_iter_t it;

    if (bson_iter_init_find(&it, b, key.c_str()) && bson_iter_type(&it) == bson_type) {
        return get_bson_value<T>(&it);
    } else if (is_required) {
        throw std::runtime_error("get_simple_field() err : no field " + key);
    }
}

}