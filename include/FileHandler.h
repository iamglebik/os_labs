#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "employee.h"
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;

class FileHandler {
public:
    static std::vector<Employee> readBinaryFile(const std::string& filename) {
        std::vector<Employee> employees;
        std::ifstream file(filename, std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }

        Employee emp;
        while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
            employees.push_back(emp);
        }

        if (file.bad()) {
            throw std::runtime_error("Error reading file: " + filename);
        }

        return employees;
    }

    static void writeBinaryFile(const std::string& filename,
        const std::vector<Employee>& employees) {
        std::ofstream file(filename, std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Cannot create file: " + filename);
        }

        for (const auto& emp : employees) {
            file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
            if (!file.good()) {
                throw std::runtime_error("Error writing to file: " + filename);
            }
        }
    }

    static bool fileExists(const std::string& filename) {
        return fs::exists(filename);
    }

    static void createDirectory(const std::string& path) {
        if (!fs::exists(path)) {
            fs::create_directories(path);
        }
    }
};

#endif