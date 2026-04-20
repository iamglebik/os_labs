#include <iostream>
#include "employee.h"
#include "FileHandler.h"
#include "ReportGenerator.h"
#include "InputValidator.h"

class ReportProcessor {
private:
    std::vector<Employee> employees;
    double hourlyRate{ 0.0 };

public:
    bool loadData(const std::string& filename) {
        try {
            employees = FileHandler::readBinaryFile(filename);
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading data: " << e.what() << std::endl;
            return false;
        }
    }

    void setHourlyRate(double rate) {
        if (rate < MIN_HOURLY_RATE || rate > MAX_HOURLY_RATE) {
            throw std::invalid_argument("Invalid hourly rate");
        }
        hourlyRate = rate;
    }

    void generateReport(const std::string& reportFile) {
        if (employees.empty()) {
            std::cout << "Warning: No employees to report." << std::endl;
        }

        std::cout << "\n=== Employee Report ===" << std::endl;
        ReportGenerator::printToConsole(employees, hourlyRate);

        try {
            ReportGenerator::writeToFile(reportFile, employees, hourlyRate);
            std::cout << "\n✓ Report saved to: " << reportFile << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error saving report: " << e.what() << std::endl;
            throw;
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        std::string binaryFile, reportFile;
        double hourlyRate;

        if (argc == 4) {
            binaryFile = argv[1];
            reportFile = argv[2];
            hourlyRate = std::stod(argv[3]);
        }
        else {
            std::cout << "=== Report Generator ===" << std::endl;
            binaryFile = InputValidator::getName("Binary file name: ");
            reportFile = InputValidator::getName("Report file name: ");
            hourlyRate = InputValidator::getDouble("Hourly rate: ",
                MIN_HOURLY_RATE,
                MAX_HOURLY_RATE);
        }

        FileHandler::createDirectory("reports");
        if (reportFile.find('/') == std::string::npos) {
            reportFile = "reports/" + reportFile;
        }

        ReportProcessor processor;

        if (!processor.loadData(binaryFile)) {
            return 1;
        }

        processor.setHourlyRate(hourlyRate);
        processor.generateReport(reportFile);

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}