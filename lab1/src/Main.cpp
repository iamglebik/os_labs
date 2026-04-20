#include <iostream>
#include <cstdlib>
#include "FileHandler.h"
#include "InputValidator.h"

#ifdef _WIN32
#include <windows.h>
#endif

class ProcessLauncher {
public:
    static int runCreator(const std::string& filename, int count) {
        std::cout << "\n=== Starting Creator ===" << std::endl;
        std::string cmd = "./Creator.exe " + filename + " " + std::to_string(count);
        return system(cmd.c_str());
    }

    static int runReporter(const std::string& binFile,
        const std::string& reportFile,
        double rate) {
        std::cout << "\n=== Starting Reporter ===" << std::endl;
        std::string cmd = "./Reporter.exe " + binFile + " " +
            reportFile + " " + std::to_string(rate);
        return system(cmd.c_str());
    }
};

void printFileContent(const std::string& filename) {
    if (!FileHandler::fileExists(filename)) {
        std::cerr << "File not found: " << filename << std::endl;
        return;
    }

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "Contents of: " << filename << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    if (filename.find(".txt") != std::string::npos) {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
    }
    else {
        try {
            auto employees = FileHandler::readBinaryFile(filename);
            for (const auto& emp : employees) {
                std::cout << "ID: " << emp.num
                    << ", Name: " << emp.getName()
                    << ", Hours: " << emp.hours << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error reading file: " << e.what() << std::endl;
        }
    }
}

int main() {
    try {
        std::cout << "=== Employee Records System ===" << std::endl;
        std::cout << "Version 2.0" << std::endl;

        std::string filename = InputValidator::getName("\nBinary filename: ");
        int count = InputValidator::getInt("Number of employees: ", 1, MAX_EMPLOYEES);

        // Ensure filename has path
        if (filename.find('/') == std::string::npos) {
            filename = "data/" + filename;
        }
        FileHandler::createDirectory("data");

        int result = ProcessLauncher::runCreator(filename, count);

        if (result == 0) {
            std::cout << "\n✓ Creator completed successfully!" << std::endl;
            printFileContent(filename);

            std::string reportFile = InputValidator::getName("\nReport filename: ");
            double rate = InputValidator::getDouble("Hourly rate: ",
                MIN_HOURLY_RATE,
                MAX_HOURLY_RATE);

            if (reportFile.find('/') == std::string::npos) {
                reportFile = "reports/" + reportFile;
            }
            FileHandler::createDirectory("reports");

            result = ProcessLauncher::runReporter(filename, reportFile, rate);

            if (result == 0) {
                std::cout << "\n✓ Reporter completed successfully!" << std::endl;
                printFileContent(reportFile);
                std::cout << "\n✓ All operations completed successfully!" << std::endl;
            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}