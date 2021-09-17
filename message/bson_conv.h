#ifndef GAIJIN_GAMES_BSON_CONV_H
#define GAIJIN_GAMES_BSON_CONV_H

#pragma once

#include <string>
#include <vector>

#include "bson/bson.h"

namespace bson_conv {

template<typename T>
inline bson_type_t get_bson_type() {
    throw std::runtime_error("get_bson_type() bad cast ");
}

template<>
inline bson_type_t get_bson_type<std::string>() {return BSON_TYPE_UTF8; }

template<>
inline bson_type_t get_bson_type<int64_t>() {return BSON_TYPE_INT64; }

template<typename T>
inline T get_bson_value(const bson_iter_t*){
    throw std::runtime_error("get_bson_value() bad cast ");
}

template<>
inline std::string get_bson_value<std::string>(const bson_iter_t* it) {
    uint32_t len;
    return {bson_iter_utf8(it, &len)};
}

template<>
inline int64_t get_bson_value<int64_t>(const bson_iter_t* it) {
    return bson_iter_int64(it);
}

template<typename T>
inline T get_simple_field(const bson_t* b, const std::string& key, bool is_required, T default_val = T()) {
    bson_type_t bson_type = get_bson_type<T>();
    bson_iter_t it;

    if (bson_iter_init_find(&it, b, key.c_str()) && bson_iter_type(&it) == bson_type) {
        return get_bson_value<T>(&it);
    }
    else if (is_required) {
        throw std::runtime_error("get_simple_field() err : no field " + key);
    }

    return default_val;
}

template<typename T>
inline void get_simple_field(T& dst, const bson_t* b, const std::string& key, bool is_required, T default_val = T()){
    dst = get_simple_field<T>(b, key, is_required, default_val);
}

template<typename T>
inline void append_simple_field(bson_t *, const std::string&, const T&){
    throw std::runtime_error("append_simple_field() bad cast ");
}

template<>
inline void append_simple_field<std::string>(bson_t* b, const std::string& key, const std::string& val) {
    if (!BSON_APPEND_UTF8(b, key.c_str(), val.c_str())) {
        throw std::runtime_error("append_simple_field<std::string> err : can't append " + key);
    }
}

template<>
inline void append_simple_field<int64_t>(bson_t* b, const std::string& key, const int64_t & val) {
    if (!BSON_APPEND_INT64(b, key.c_str(), val)) {
        throw std::runtime_error("append_simple_field<int64_t> err : can't append " + key);
    }
}

}


#endif //GAIJIN_GAMES_BSON_CONV_H
