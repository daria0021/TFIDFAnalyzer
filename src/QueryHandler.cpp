#include "QueryHandler.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <numeric>  // ← ЭТО СТРОКА НУЖНА ДЛЯ std::accumulate

QueryHandler::QueryHandler(const TFIDFCalculator& calc) : calculator(calc) {}

std::vector<std::pair<std::string, double>> QueryHandler::getTopWords(const DocumentInfo& doc, int topN) const {
    std::vector<std::pair<std::string, double>> wordsWithTFIDF;

    std::transform(doc.wordCounts.begin(), doc.wordCounts.end(),
                   std::back_inserter(wordsWithTFIDF),
                   [this, &doc](const auto& pair) {
                       return std::make_pair(pair.first,
                              calculator.getTFIDF(pair.first, doc.filename));
                   });

    std::sort(wordsWithTFIDF.begin(), wordsWithTFIDF.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });

    if (wordsWithTFIDF.size() > static_cast<size_t>(topN)) {
        wordsWithTFIDF.resize(topN);
    }

    return wordsWithTFIDF;
}

void QueryHandler::handleWordQuery(const std::string& word) {
    int docsWithWord = calculator.getDocumentCountWithWord(word);
    double idf = calculator.getIDF(word);

    std::cout << "Word: " << word << std::endl;
    std::cout << "Documents total: " << calculator.getTotalDocuments() << std::endl;
    std::cout << "Documents with word: " << docsWithWord << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "IDF: " << idf << std::endl;
    std::cout << "Appears in:" << std::endl;

    const auto& docs = calculator.getDocuments();
    std::for_each(docs.begin(), docs.end(), [&word, this](const DocumentInfo& doc) {
        if (calculator.getTF(word, doc.filename) > 0) {
            std::cout << " - " << doc.filename << std::endl;
        }
    });
}

void QueryHandler::handleWordInDocQuery(const std::string& word, const std::string& filename) {
    double tf = calculator.getTF(word, filename);
    double idf = calculator.getIDF(word);
    double tfidf = calculator.getTFIDF(word, filename);
    const DocumentInfo* doc = calculator.getDocumentInfo(filename);

    if (!doc) {
        std::cout << "Error: Document " << filename << " not found" << std::endl;
        return;
    }

    int count = 0;
    auto it = doc->wordCounts.find(word);
    if (it != doc->wordCounts.end()) {
        count = it->second;
    }

    std::cout << "Word: " << word << std::endl;
    std::cout << "Document: " << filename << std::endl;
    std::cout << "Count: " << count << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "TF: " << tf << std::endl;
    std::cout << "TF-IDF: " << tfidf << std::endl;
}

void QueryHandler::handleDocQuery(const std::string& filename) {
    const DocumentInfo* doc = calculator.getDocumentInfo(filename);

    if (!doc) {
        std::cout << "Error: Document " << filename << " not found" << std::endl;
        return;
    }

    std::cout << "Document: " << filename << std::endl;
    std::cout << "Total words: " << doc->totalWords << std::endl;
    std::cout << "Unique words: " << doc->uniqueWords << std::endl;
    std::cout << "Top words:" << std::endl;

    auto topWords = getTopWords(*doc, 5);
    int rank = 1;
    std::for_each(topWords.begin(), topWords.end(), [&rank](const auto& pair) {
        std::cout << " " << rank++ << ". " << pair.first
                  << " (" << std::fixed << std::setprecision(4) << pair.second << ")" << std::endl;
    });
}

void QueryHandler::handleSearchQuery(const std::vector<std::string>& words) {
    std::vector<std::pair<std::string, double>> docScores;
    const auto& docs = calculator.getDocuments();

    std::transform(docs.begin(), docs.end(), std::back_inserter(docScores),
                   [this, &words](const DocumentInfo& doc) {
                       double totalScore = std::accumulate(words.begin(), words.end(), 0.0,
                           [this, &doc](double sum, const std::string& word) {
                               return sum + calculator.getTFIDF(word, doc.filename);
                           });
                       return std::make_pair(doc.filename, totalScore);
                   });

    std::sort(docScores.begin(), docScores.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;
              });

    std::cout << "Query:";
    std::for_each(words.begin(), words.end(), [](const std::string& word) {
        std::cout << " " << word;
    });
    std::cout << std::endl;
    std::cout << "Results:" << std::endl;

    int rank = 1;
    std::for_each(docScores.begin(), docScores.end(), [&rank](const auto& pair) {
        if (pair.second > 0) {
            std::cout << " " << rank++ << ". " << pair.first
                      << " (" << std::fixed << std::setprecision(4) << pair.second << ")" << std::endl;
        }
    });
}

void QueryHandler::processQuery(const std::string& queryLine) {
    if (queryLine.empty()) return;

    std::istringstream iss(queryLine);
    std::string command;
    iss >> command;

    if (command == "WORD") {
        std::string word;
        iss >> word;
        if (word.empty()) {
            std::cout << "Error: Invalid WORD query format" << std::endl;
            return;
        }
        handleWordQuery(word);
    }
    else if (command == "WORD_IN_DOC") {
        std::string word, filename;
        iss >> word >> filename;
        if (word.empty() || filename.empty()) {
            std::cout << "Error: Invalid WORD_IN_DOC query format" << std::endl;
            return;
        }
        handleWordInDocQuery(word, filename);
    }
    else if (command == "DOC") {
        std::string filename;
        iss >> filename;
        if (filename.empty()) {
            std::cout << "Error: Invalid DOC query format" << std::endl;
            return;
        }
        handleDocQuery(filename);
    }
    else if (command == "QUERY") {
        std::vector<std::string> words;
        std::string word;
        while (iss >> word) {
            words.push_back(word);
        }
        if (words.empty()) {
            std::cout << "Error: Invalid QUERY query format" << std::endl;
            return;
        }
        handleSearchQuery(words);
    }
    else {
        std::cout << "Error: Unknown query type: " << command << std::endl;
    }
}