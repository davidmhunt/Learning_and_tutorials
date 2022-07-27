#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include "parseJSON/parseJson.hpp"

using json = nlohmann::json;

int main(void) {

    std::string file_name = "/home/david/Documents/Learning_and_tutorials/cpp/JSON_parse_test/aSimpleJson.json";

    json parsed_data = parseJson::parse_JSON(file_name);
    std::cout << "\n\nFull File Contents\n";
    parseJson::print_file(file_name);

    return EXIT_SUCCESS; 
}
