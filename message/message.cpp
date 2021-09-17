#include <iostream>

#include "message.h"

namespace message_ {

void request_message::print() const {
    std::cout << "\"command\" : " << "\"" << command << "\"" << std::endl;
    std::cout << "\"key\"     : " << "\"" << key     << "\""<< std::endl;
    std::cout << "\"value\"   : " << "\"" << value   << "\""<< std::endl;
}

void answer_message::print() const{
    std::cout << key << "=" << value << std::endl;
    std::cout << "reads="         << stat.reads << std::endl;
    std::cout << "writes="        << stat.writes << std::endl;
}
}