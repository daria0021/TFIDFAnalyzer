#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

#include "TFIDFCalculator.h"
#include <string>
#include <vector>

class QueryHandler {
private:
    const TFIDFCalculator& calculator;

    void handleWordQuery(const std::string& word);
    void handleWordInDocQuery(const std::string& word, const std::string& filename);
    void handleDocQuery(const std::string& filename);
    void handleSearchQuery(const std::vector<std::string>& words);

    std::vector<std::pair<std::string, double>> getTopWords(const DocumentInfo& doc, int topN = 5) const;

public:
    QueryHandler(const TFIDFCalculator& calc);
    void processQuery(const std::string& queryLine);
};

#endif