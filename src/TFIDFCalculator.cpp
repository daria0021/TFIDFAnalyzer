#include "TFIDFCalculator.h"
#include "TextProcessor.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>

void TFIDFCalculator::loadDocuments(const std::vector<std::string>& filenames) {
    documents.clear();
    allWords.clear();
    totalDocuments = filenames.size();

    std::transform(filenames.begin(), filenames.end(), std::back_inserter(documents),
                   [](const std::string& filename) {
                       DocumentInfo doc;
                       doc.filename = filename;
                       doc.wordCounts.clear();

                       std::ifstream file(filename);
                       if (!file.is_open()) {
                           std::cerr << "Warning: Cannot open file " << filename << std::endl;
                           return doc;
                       }

                       std::string content((std::istreambuf_iterator<char>(file)),
                                           std::istreambuf_iterator<char>());
                       file.close();

                       std::string lowerContent = TextProcessor::toLower(content);
                       std::string cleanContent = TextProcessor::removePunctuation(lowerContent);
                       std::vector<std::string> tokens = TextProcessor::tokenize(cleanContent);

                       doc.totalWords = tokens.size();
                       doc.wordCounts = TextProcessor::getWordCounts(tokens);
                       doc.uniqueWords = doc.wordCounts.size();

                       return doc;
                   });

    std::for_each(documents.begin(), documents.end(), [this](const DocumentInfo& doc) {
        std::transform(doc.wordCounts.begin(), doc.wordCounts.end(),
                       std::inserter(allWords, allWords.begin()),
                       [](const auto& pair) { return pair.first; });
    });

    computeDocumentFrequency();
    computeTFIDF();
}

void TFIDFCalculator::computeDocumentFrequency() {
    documentFrequency.clear();

    std::for_each(allWords.begin(), allWords.end(), [this](const std::string& word) {
        int count = std::count_if(documents.begin(), documents.end(),
                                   [&word](const DocumentInfo& doc) {
                                       return doc.wordCounts.find(word) != doc.wordCounts.end();
                                   });
        documentFrequency[word] = count;
    });
}

double TFIDFCalculator::calculateIDF(const std::string& word) const {
    auto it = documentFrequency.find(word);
    if (it == documentFrequency.end() || it->second == 0) {
        return 0.0;
    }
    return std::log(static_cast<double>(totalDocuments) / it->second);
}

double TFIDFCalculator::calculateTF(const std::string& word, const DocumentInfo& doc) const {
    auto it = doc.wordCounts.find(word);
    if (it == doc.wordCounts.end() || doc.totalWords == 0) {
        return 0.0;
    }
    return static_cast<double>(it->second) / doc.totalWords;
}

void TFIDFCalculator::computeTFIDF() {
    std::for_each(documents.begin(), documents.end(), [this](DocumentInfo& doc) {
        std::for_each(doc.wordCounts.begin(), doc.wordCounts.end(),
                      [this, &doc](const auto& pair) {
                          const std::string& word = pair.first;
                          double tf = calculateTF(word, doc);
                          doc.tfValues[word] = tf;
                      });
    });
}

double TFIDFCalculator::getTF(const std::string& word, const std::string& filename) const {
    const DocumentInfo* doc = getDocumentInfo(filename);
    if (!doc) return 0.0;
    auto it = doc->tfValues.find(word);
    return (it != doc->tfValues.end()) ? it->second : 0.0;
}

double TFIDFCalculator::getIDF(const std::string& word) const {
    return calculateIDF(word);
}

double TFIDFCalculator::getTFIDF(const std::string& word, const std::string& filename) const {
    double tf = getTF(word, filename);
    double idf = getIDF(word);
    return tf * idf;
}

int TFIDFCalculator::getDocumentCountWithWord(const std::string& word) const {
    auto it = documentFrequency.find(word);
    return (it != documentFrequency.end()) ? it->second : 0;
}

const DocumentInfo* TFIDFCalculator::getDocumentInfo(const std::string& filename) const {
    auto it = std::find_if(documents.begin(), documents.end(),
                           [&filename](const DocumentInfo& doc) {
                               return doc.filename == filename;
                           });
    return (it != documents.end()) ? &(*it) : nullptr;
}