#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include "employee.h"
#include <iostream>
#include <string>
#include <limits>

class InputValidator {
private:
    static void clearInputStream() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

public:
    static int getInt(const std::string& prompt, int min, int max) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value && value >= min && value <= max) {
                clearInputStream();
                return value;
            }
            std::cerr << "Invalid input. Please enter a number between "
                << min << " and " << max << std::endl;
            clearInputStream();
        }
    }

    static double getDouble(const std::string& prompt, double min, double max) {
        double value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value && value >= min && value <= max) {
                clearInputStream();
                return value;
            }
            std::cerr << "Invalid input. Please enter a number between "
                << min << " and " << max << std::endl;
            clearInputStream();
        }
    }

    static std::string getName(const std::string& prompt) {
        std::string name;
        while (true) {
            std::cout << prompt;
            std::cin >> name;
            clearInputStream();

            if (name.length() < MAX_NAME_LENGTH && !name.empty()) {
                return name;
            }
            std::cerr << "Name must be 1-" << (MAX_NAME_LENGTH - 1)
                << " characters." << std::endl;
        }
    }
};

#endif