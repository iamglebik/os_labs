#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class TestUtils {
public:
    static std::string createUniqueTestDir() {
        return "test_dir_" + std::to_string(time(nullptr));
    }

    static void createTestDirectory(const std::string& path) {
        fs::create_directories(path);
    }

    static void cleanupTestDirectory(const std::string& path) {
        fs::remove_all(path);
    }

    static void createInputFile(const std::string& filename,
        const std::vector<std::string>& inputs) {
        std::ofstream file(filename);
        for (const auto& input : inputs) {
            file << input << "\n";
        }
    }

    static std::vector<std::string> readFileLines(const std::string& filename) {
        std::vector<std::string> lines;
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        return lines;
    }

    static bool fileExists(const std::string& filename) {
        return fs::exists(filename);
    }

    static size_t fileSize(const std::string& filename) {
        return fs::file_size(filename);
    }
};

#endif