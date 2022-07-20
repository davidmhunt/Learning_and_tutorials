/*
Simple file for testing YAML input 
*/

#include <iostream>
#include <cstdlib>
#include "yaml-cpp/yaml.h"

int main (void){
    YAML::Node config = YAML::LoadFile("config_simple.yaml");
    std::cout << "doe: " << config["doe"].as<std::string>() << "\n";
}