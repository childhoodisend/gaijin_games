#ifndef GAIJIN_GAMES_BSON_CONV_H
#define GAIJIN_GAMES_BSON_CONV_H

#pragma once

#include <string>

#include "bson/bson.h"

namespace bson_conv {

template<typename T>
bson_type_t get_bson_type();

template<typename T>
T get_bson_value(const bson_iter_t* it);

template<typename T>
T get_simple_field(const bson_t* b, const std::string& key, bool is_required);

}


#endif //GAIJIN_GAMES_BSON_CONV_H
