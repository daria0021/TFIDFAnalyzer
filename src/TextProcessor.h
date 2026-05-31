#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

class TextProcessor {
public:
    static std::string toLower(const std::string& text);
    static std::string removePunctuation(const std::string& text);
    static std::vector<std::string> tokenize(const std::string& text);
    static std::unordered_map<std::string, int> getWordCounts(const std::vector<std::string>& tokens);
    static std::set<std::string> getUniqueWords(const std::unordered_map<std::string, int>& wordCounts);
};

#endif