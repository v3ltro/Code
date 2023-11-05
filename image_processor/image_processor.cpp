#include "Headers/Image.h"
#include "Headers/Console.h"
#include "Headers/Filters.h"

int main(int argc, char** argv) {
    Console console(argc, argv);
    if (argc == 1) {
        return 0;
    }
    Image open(0, 0);
    std::ifstream ifs(console.parsed_.input_path, std::ifstream::in | std::ios::binary);
    if (!ifs.is_open()) {
        throw(InputArgumentException("Wrong input path\n"));
    }
    open.Read(ifs);
    std::ofstream ofs(console.parsed_.output_path, std::ofstream::out | std::ios::binary);
    if (!ofs.is_open()) {
        throw(InputArgumentException("Wrong output path\n"));
    }
    FilterChain(open, console.parsed_.filters);
    open.Export(ofs);
}