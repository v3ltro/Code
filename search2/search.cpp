#include "search.h"

#include <string>
#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <cmath>
#include <string_view>

bool LowerEqual(const std::string_view& str1, const std::string_view& str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    for (size_t i = 0; i < str1.size(); ++i) {
        if (std::tolower(str1[i]) != str2[i]) {
            return false;
        }
    }
    return true;
}

std::pair<size_t, size_t> FindStringBoundaries(const std::string_view& text, size_t string_number) {
    bool not_empty = false;
    bool found_first = false;
    size_t strings = 0;
    std::pair<size_t, size_t> ans;
    for (size_t i = 0; i <= text.size(); ++i) {
        if (strings == string_number) {
            if (!found_first && text[i] != '\n') {
                found_first = true;
                ans.first = i;
            }
            ans.second = i;
        }
        if (text[i] != '\n') {
            not_empty = true;
        } else {
            strings += not_empty;
            not_empty = false;
        }
    }
    return ans;
}

std::set<std::string> NormalizeQuery(const std::string_view& input) {
    std::set<std::string> words;
    std::string word;
    for (size_t i = 0; i < input.size(); ++i) {
        if (!std::isalpha(input[i])) {
            if (!word.empty()) {
                words.insert(word);
            }
            word.clear();
        } else {
            word += static_cast<char>(std::tolower(input[i]));
        }
    }
    if (!word.empty()) {
        words.insert(word);
    }
    return words;
}

std::vector<std::vector<std::string_view>> NormalizeText(const std::string_view& text,
                                                         bool include_punctuation = true) {
    size_t index = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] != '\n') {
            index = i;
            break;
        }
    }
    std::vector<std::vector<std::string_view>> res;
    std::vector<std::string_view> vec;
    for (size_t i = index; i < text.size(); ++i) {
        if (text[i] == '\n') {
            if (index != i) {
                vec.emplace_back(std::string_view(text.data() + index, i - index));
            }
            index = i + 1;
            if (!vec.empty()) {
                res.push_back(vec);
            }
            vec.clear();
        } else {
            if (!std::isalpha(text[i])) {
                if (index != i) {
                    vec.emplace_back(std::string_view(text.data() + index, i - index));
                }
                index = i + 1;
            }
        }
    }
    if (index != text.size()) {
        vec.push_back(std::string_view(text.data() + index, text.size() - index));
    }
    if (!vec.empty()) {
        res.push_back(vec);
    }
    return res;
}

double CalculateIdf(const std::vector<std::vector<std::string_view>>& normalized_text, const std::string_view& query) {
    size_t count = 0;
    for (size_t i = 0; i < normalized_text.size(); ++i) {
        bool flag = false;
        for (const auto& str : normalized_text[i]) {
            flag |= LowerEqual(str, query);
        }
        count += flag;
    }
    return count == 0 ? 0 : std::log(static_cast<double>(normalized_text.size()) / static_cast<double>(count));
}

double CalculateTf(const std::vector<std::string_view>& str, const std::string_view& query) {
    int count_query = 0;
    for (const auto& s : str) {
        count_query += LowerEqual(s, query);
    }
    return str.empty() ? 0 : static_cast<double>(count_query) / static_cast<double>(str.size());
}

std::vector<std::pair<double, size_t>> CalculateTfIdf(const std::vector<std::vector<std::string_view>>& normalized_text,
                                                      const std::string_view& query) {
    std::set<std::string> normalized_query = NormalizeQuery(query);
    std::vector<std::pair<double, size_t>> results(normalized_text.size());
    for (size_t i = 0; i < results.size(); ++i) {
        results[i].first = 0;
        results[i].second = i;
    }
    for (const auto& str : normalized_query) {
        double idf = CalculateIdf(normalized_text, str);
        for (size_t i = 0; i < normalized_text.size(); ++i) {
            double tf = CalculateTf(normalized_text[i], str);
            results[i].first -= idf * tf;
        }
    }
    std::sort(results.begin(), results.end());
    return results;
}

std::vector<std::string_view> SearchHelp(std::string_view text, std::string_view query, size_t results_count) {
    std::vector<std::string_view> res;
    std::vector<std::vector<std::string_view>> normalized_text = NormalizeText(text);
    std::vector<std::pair<double, size_t>> tf_idf = CalculateTfIdf(normalized_text, query);
    std::string str;
    for (size_t i = 0; i < results_count; ++i) {
        if (i < tf_idf.size() && tf_idf[i].first < 0) {
            std::pair<size_t, size_t> boundaries = FindStringBoundaries(text, tf_idf[i].second);
            res.push_back(std::string_view(text.data() + boundaries.first, boundaries.second - boundaries.first));
        }
    }
    return res;
}

void SearchEngine::BuildIndex(std::string_view text) {
    text_ = text;
}

std::vector<std::string_view> SearchEngine::Search(std::string_view query, size_t results_count) const {
    return SearchHelp(text_, query, results_count);
}