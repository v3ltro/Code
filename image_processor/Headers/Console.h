#pragma once

#include <vector>
#include <iostream>
#include <string>

struct ParsedCommands {
    const char* input_path;
    const char* output_path;
    std::vector<std::pair<std::string, std::vector<std::string_view>>> filters;
};

class Console {
public:
    Console(int argc, char** argv);
    ParsedCommands parsed_;
};