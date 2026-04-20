#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include "employee.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

class ReportGenerator {
private:
    static constexpr int ID_WIDTH = 8;
    static constexpr int NAME_WIDTH = 12;
    static constexpr int HOURS_WIDTH = 10;
    static constexpr int SALARY_WIDTH = 12;

public:
    static void printToConsole(const std::vector<Employee>& employees,
        double hourlyRate) {
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << std::left
            << std::setw(ID_WIDTH) << "ID"
            << std::setw(NAME_WIDTH) << "Name"
            << std::setw(HOURS_WIDTH) << "Hours"
            << std::setw(SALARY_WIDTH) << "Salary" << std::endl;
        std::cout << std::string(50, '-') << std::endl;

        double totalSalary = 0.0;
        for (const auto& emp : employees) {
            double salary = emp.calculateSalary(hourlyRate);
            totalSalary += salary;

            std::cout << std::left
                << std::setw(ID_WIDTH) << emp.num
                << std::setw(NAME_WIDTH) << emp.getName()
                << std::setw(HOURS_WIDTH) << std::fixed
                << std::setprecision(1) << emp.hours
                << "$" << std::fixed << std::setprecision(2)
                << salary << std::endl;
        }

        std::cout << std::string(50, '-') << std::endl;
        std::cout << "Total: $" << std::fixed << std::setprecision(2)
            << totalSalary << std::endl;
    }

    static void writeToFile(const std::string& filename,
        const std::vector<Employee>& employees,
        double hourlyRate) {
        std::ofstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error("Cannot create report file: " + filename);
        }

        file << "ID,Name,Hours,Salary" << std::endl;

        for (const auto& emp : employees) {
            double salary = emp.calculateSalary(hourlyRate);
            file << emp.num << ","
                << emp.getName() << ","
                << emp.hours << ","
                << static_cast<int>(salary) << std::endl;
        }
    }
};

#endif