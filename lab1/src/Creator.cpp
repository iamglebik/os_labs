#include <iostream>
#include <memory>
#include <vector>
#include "employee.h"
#include "FileHandler.h"
#include "InputValidator.h"

class EmployeeCreator {
private:
    std::vector<Employee> employees;

    Employee inputEmployee(int id) {
        Employee emp;
        emp.num = id;

        std::cout << "\n--- Employee #" << id << " ---" << std::endl;

        std::string name = InputValidator::getName("Name: ");
        emp.setName(name);

        emp.hours = InputValidator::getDouble("Hours worked: ",
            MIN_HOURS, MAX_HOURS);

        return emp;
    }

public:
    void createEmployees(int count) {
        employees.clear();
        employees.reserve(count);

        for (int i = 1; i <= count; ++i) {
            employees.push_back(inputEmployee(i));
        }
    }

    void saveToFile(const std::string& filename) {
        FileHandler::writeBinaryFile(filename, employees);
        std::cout << "\n✓ Successfully saved " << employees.size()
            << " employees to " << filename << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        std::string filename;
        int recordCount;

        if (argc == 3) {
            filename = argv[1];
            recordCount = std::stoi(argv[2]);
        }
        else {
            std::cout << "=== Employee Data Creator ===" << std::endl;
            filename = InputValidator::getName("Enter filename: ");
            recordCount = InputValidator::getInt("Number of employees: ",
                1, MAX_EMPLOYEES);
        }

        FileHandler::createDirectory("data");
        if (filename.find('/') == std::string::npos) {
            filename = "data/" + filename;
        }

        EmployeeCreator creator;
        creator.createEmployees(recordCount);
        creator.saveToFile(filename);

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}