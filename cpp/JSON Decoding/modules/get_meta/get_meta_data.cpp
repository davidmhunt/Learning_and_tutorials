#include <iostream>
#include <iomanip>

#include <nlohmann/json.hpp>

#include "get_meta_data.hpp"

using nlohmann::json;

void get_Meta::print_Meta(void){
    std::cout << std::setw(4) << json::meta() << std::endl;
    
};