#ifndef TFIDFCALCULATOR_H
#define TFIDFCALCULATOR_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

struct DocumentInfo {
    std::string filename;
    int totalWords;
    int uniqueWords;
    std::unordered_map<std::string, int> wordCounts;
    std::unordered_map<std::string, double> tfValues;
};

class TFIDFCalculator {
private:
    std::vector<DocumentInfo> documents;
    std::set<std::string> allWords;
    std::unordered_map<std::string, int> documentFrequency;
    int totalDocuments;

    void computeDocumentFrequency();
    double calculateIDF(const std::string& word) const;
    double calculateTF(const std::string& word, const DocumentInfo& doc) const;

public:
    void loadDocuments(const std::vector<std::string>& filenames);
    void computeTFIDF();

    double getTF(const std::string& word, const std::string& filename) const;
    double getIDF(const std::string& word) const;
    double getTFIDF(const std::string& word, const std::string& filename) const;
    int getDocumentCountWithWord(const std::string& word) const;
    const std::vector<DocumentInfo>& getDocuments() const { return documents; }
    const std::set<std::string>& getAllWords() const { return allWords; }
    int getTotalDocuments() const { return totalDocuments; }
    const DocumentInfo* getDocumentInfo(const std::string& filename) const;
};

#endif