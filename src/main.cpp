#include "TFIDFCalculator.h"
#include "QueryHandler.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

std::string findDocumentsFile() {
    std::vector<std::string> possiblePaths = {
        "documents.txt",
        "../documents.txt",
        "../../documents.txt",
        "./documents.txt",
        "cmake-build-debug/documents.txt",
        "../cmake-build-debug/documents.txt",
        "../../cmake-build-debug/documents.txt",
    };

    for (const auto& path : possiblePaths) {
        std::ifstream test(path);
        if (test.is_open()) {
            test.close();
            std::cout << "Found documents.txt at: " << path << std::endl;
            return path;
        }
    }

    // Поиск рекурсивно
    for (const auto& entry : fs::recursive_directory_iterator(".")) {
        if (entry.path().filename() == "documents.txt") {
            std::cout << "Found documents.txt at: " << entry.path().string() << std::endl;
            return entry.path().string();
        }
    }

    return "documents.txt";
}

std::string findDocumentFile(const std::string& relativePath) {
    // Проверяем различные возможные расположения документа
    std::vector<std::string> possiblePaths = {
        relativePath,                                    // Относительно рабочей директории
        "../" + relativePath,                            // На уровень выше
        "../../" + relativePath,                         // Два уровня выше
        "./" + relativePath,                             // Текущая директория
        "C:/Users/lipen/CLionProjects/TFIDFAnalyzer/" + relativePath,  // Абсолютный путь
        "C:/Users/Lipen/CLionProjects/TFIDFAnalyzer/" + relativePath,  // Альтернативный путь
    };

    for (const auto& path : possiblePaths) {
        std::ifstream test(path);
        if (test.is_open()) {
            test.close();
            return path;
        }
    }

    return relativePath;  // Вернем исходный путь, если не нашли
}

std::vector<std::string> loadDocumentList(const std::string& filename) {
    std::vector<std::string> filenames;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open " << filename << std::endl;
        return filenames;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Удаляем пробелы
        line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        line.erase(std::find_if(line.rbegin(), line.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), line.end());

        if (!line.empty()) {
            // Находим реальный путь к документу
            std::string realPath = findDocumentFile(line);
            std::cout << "Document path resolved: " << line << " -> " << realPath << std::endl;
            filenames.push_back(realPath);
        }
    }

    file.close();
    return filenames;
}

int main() {
    std::cout << "TF-IDF Text Analyzer" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << std::endl;

    // Показываем текущую рабочую директорию
    std::cout << "Current working directory: " << fs::current_path() << std::endl;
    std::cout << std::endl;

    // Ищем файл documents.txt
    std::string docListPath = findDocumentsFile();

    // Загружаем список документов
    std::vector<std::string> filenames = loadDocumentList(docListPath);

    if (filenames.empty()) {
        std::cerr << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "FATAL ERROR: No documents found!" << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << std::endl;
        std::cerr << "Please ensure:" << std::endl;
        std::cerr << "1. documents.txt exists with document paths" << std::endl;
        std::cerr << "2. The document files exist" << std::endl;
        std::cerr << std::endl;

        std::cout << "Press Enter to exit...";
        std::cin.get();
        return 1;
    }

    std::cout << "\nLoading " << filenames.size() << " documents..." << std::endl;
    std::cout << std::endl;

    TFIDFCalculator calculator;
    calculator.loadDocuments(filenames);

    std::cout << "\nDocuments loaded successfully!" << std::endl;
    std::cout << "Total unique words: " << calculator.getAllWords().size() << std::endl;
    std::cout << std::endl;

    if (calculator.getAllWords().empty()) {
        std::cout << "WARNING: No words were loaded. Please check that document files contain text." << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Available commands:" << std::endl;
    std::cout << "  WORD <word>                         - Statistics for a word across all documents" << std::endl;
    std::cout << "  WORD_IN_DOC <word> <document>       - Statistics for a word in a specific document" << std::endl;
    std::cout << "  DOC <document>                      - Statistics for a document (top 5 words)" << std::endl;
    std::cout << "  QUERY <word1> <word2> ...           - Search for documents matching query" << std::endl;
    std::cout << "  EXIT                                - Exit the program" << std::endl;
    std::cout << std::endl;

    QueryHandler handler(calculator);
    std::string query;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, query)) {
            break;
        }

        query.erase(query.begin(), std::find_if(query.begin(), query.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        query.erase(std::find_if(query.rbegin(), query.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), query.end());

        if (query.empty()) {
            continue;
        }

        if (query == "EXIT" || query == "exit" || query == "quit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        handler.processQuery(query);
        std::cout << std::endl;
    }

    return 0;
}