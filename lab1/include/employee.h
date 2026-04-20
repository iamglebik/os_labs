#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <cstring>
#include <array>
#include <stdexcept>

constexpr int MAX_NAME_LENGTH = 10;
constexpr int MAX_EMPLOYEES = 1000;
constexpr double MIN_HOURLY_RATE = 0.0;
constexpr double MAX_HOURLY_RATE = 1000.0;
constexpr int MIN_HOURS = 1;
constexpr int MAX_HOURS = 168;

struct Employee {
    int num{ 0 };
    std::array<char, MAX_NAME_LENGTH> name{};
    double hours{ 0.0 };

    void setName(const std::string& newName) {
        if (newName.length() >= MAX_NAME_LENGTH) {
            throw std::invalid_argument("Name too long");
        }
        std::strcpy(name.data(), newName.c_str());
    }

    std::string getName() const {
        return std::string(name.data());
    }

    double calculateSalary(double hourlyRate) const {
        if (hourlyRate < MIN_HOURLY_RATE || hourlyRate > MAX_HOURLY_RATE) {
            throw std::invalid_argument("Invalid hourly rate");
        }
        return hours * hourlyRate;
    }
};

#endif