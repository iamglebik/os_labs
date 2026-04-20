#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include "Array.h"
#include "threads.h"

void printArray(const std::vector<int>& arr, const std::string& title) {
    std::cout << "\n" << title << ":\n";
    for (size_t i = 0; i < arr.size(); ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Laboratory work #2: Creating Threads ===\n";
    std::cout << "Author: Siniakou Hleb, group 12\n\n";

    std::cout << "Enter the array size: ";
    int n;
    if (!(std::cin >> n) || n <= 0) {
        std::cerr << "Error: Invalid array size" << std::endl;
        return 1;
    }

    std::vector<int> v(n);
    std::cout << "Enter " << n << " integers:" << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << "Element [" << i << "]: ";
        std::cin >> v[i];
    }

    std::vector<int> originalArray = v; 
    Array data(v);
    printArray(originalArray, "Original array");
    HANDLE hMinMax = CreateThread(NULL, 0, MinMaxThread, &data, 0, NULL);
    if (hMinMax == NULL) {
        std::cerr << "Error: Failed to create min_max thread" << std::endl;
        return 1;
    }

    HANDLE hAverage = CreateThread(NULL, 0, AverageThread, &data, 0, NULL);
    if (hAverage == NULL) {
        std::cerr << "Error: Failed to create average thread" << std::endl;
        CloseHandle(hMinMax);
        return 1;
    }

    std::cout << "\n--- Threads execution ---" << std::endl;
    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    std::cout << "\n--- Calculation results ---" << std::endl;
    std::cout << "Minimum element: " << data.minVal << std::endl;
    std::cout << "Maximum element: " << data.maxVal << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average value: " << data.avg << std::endl;

    int roundedAvg = static_cast<int>(data.avg + 0.5);
    std::cout << "\nReplacing min and max elements with rounded average: "
        << roundedAvg << std::endl;

    data.makeResArray(roundedAvg);

    printArray(data.arr, "Result array");

    std::cout << "\n=== Program completed successfully ===" << std::endl;
    return 0;
}