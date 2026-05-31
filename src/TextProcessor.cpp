#include "TextProcessor.h"
#include <algorithm>
#include <cctype>
#include <sstream>

std::string TextProcessor::toLower(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string TextProcessor::removePunctuation(const std::string& text) {
    std::string result;
    std::copy_if(text.begin(), text.end(), std::back_inserter(result),
                 [](char c) { return std::isalnum(static_cast<unsigned char>(c)) || std::isspace(c); });
    return result;
}

std::vector<std::string> TextProcessor::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        if (!word.empty()) {
            tokens.push_back(word);
        }
    }
    return tokens;
}

std::unordered_map<std::string, int> TextProcessor::getWordCounts(const std::vector<std::string>& tokens) {
    std::unordered_map<std::string, int> wordCounts;
    std::for_each(tokens.begin(), tokens.end(), [&wordCounts](const std::string& word) {
        wordCounts[word]++;
    });
    return wordCounts;
}

std::set<std::string> TextProcessor::getUniqueWords(const std::unordered_map<std::string, int>& wordCounts) {
    std::set<std::string> uniqueWords;
    std::transform(wordCounts.begin(), wordCounts.end(),
                   std::inserter(uniqueWords, uniqueWords.begin()),
                   [](const auto& pair) { return pair.first; });
    return uniqueWords;
}