#include "../Headers/Console.h"
#include "../Headers/Exceptions.h"

#include <set>

void PrintHelp() {
    std::cout << "Image processor.\nAccepted input format: \"input_path output_path filter1_name "
                 "filter1_args filter2_name filter2_args...\"\n"
                 "-crop width height - crops the image of integer size width x height "
                 "starting from upper-left corner\n-gs - converts image into grayscale\n"
                 "-neg - converts colors into their respective opposites\n"
                 "-sharp - sharpens the image\n-edge threshold - finds boundaries of objects in the image, "
                 "threshold is a double value\n-blur sigma - applies Gaussian blur "
                 "with standard deviation sigma, sigma is a double value.\n-noise "
                 "monochrome transparency - applies noise with a certain value of transparency "
                 "(expecting double between 0 and 1), monochrome should be \"true\" or \"false\"";
}

Console::Console(int argc, char** argv) {
    if (argc == 1) {
        PrintHelp();
    }
    if (argc == 2) {
        throw(InputArgumentException("Missing input/output path\n"));
    }
    parsed_.input_path = argv[1];
    parsed_.output_path = argv[2];
    bool flag = false;
    std::set<std::string_view> allowed_filters = {"-crop", "-gs", "-neg", "-sharp", "-edge", "-blur", "-noise"};
    std::pair<std::string, std::vector<std::string_view>> filter;
    for (int i = 3; i < argc; ++i) {
        if (allowed_filters.count(argv[i]) != 0) {
            if (flag) {
                parsed_.filters.push_back(filter);
                filter = {};
            }
            filter.first = argv[i];
            flag = true;
        } else if (flag) {
            filter.second.push_back(argv[i]);
        }
    }
    parsed_.filters.push_back(filter);
}