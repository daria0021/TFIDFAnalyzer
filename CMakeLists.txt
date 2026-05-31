cmake_minimum_required(VERSION 3.20)
project(TFIDFAnalyzer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(TFIDFAnalyzer
        src/main.cpp
        src/TextProcessor.cpp
        src/TFIDFCalculator.cpp
        src/QueryHandler.cpp
)

target_include_directories(TFIDFAnalyzer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)