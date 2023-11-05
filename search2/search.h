#pragma once

#include <string_view>
#include <vector>

class SearchEngine {
private:
    std::string_view text_;

public:
    void BuildIndex(std::string_view text);
    std::vector<std::string_view> Search(std::string_view query, size_t results_count) const;
};
