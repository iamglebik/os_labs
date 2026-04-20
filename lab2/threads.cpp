#include "threads.h"
#include <iostream>

static const DWORD SLEEP_MINMAX = 7;
static const DWORD SLEEP_AVERAGE = 12;

DWORD WINAPI MinMaxThread(LPVOID lpParam) {
    Array* data = static_cast<Array*>(lpParam);
    if (!data || data->n <= 0) return 1;

    int currMin = data->arr[0];
    int currMax = data->arr[0];

    for (int i = 1; i < data->n; ++i) {
        if (data->arr[i] < currMin) {
            currMin = data->arr[i];
        }
        Sleep(SLEEP_MINMAX);

        if (data->arr[i] > currMax) {
            currMax = data->arr[i];
        }
        Sleep(SLEEP_MINMAX); 
    }

    data->minVal = currMin;
    data->maxVal = currMax;

    std::cout << "[Thread min_max] Minimum: " << data->minVal
        << ", Maximum: " << data->maxVal << std::endl;
    return 0;
}

DWORD WINAPI AverageThread(LPVOID lpParam) {
    Array* data = static_cast<Array*>(lpParam);
    if (!data || data->n <= 0) return 1;

    long long sum = 0;
    for (int i = 0; i < data->n; ++i) {
        sum += data->arr[i];
        Sleep(SLEEP_AVERAGE);
    }

    data->avg = static_cast<double>(sum) / static_cast<double>(data->n);
    std::cout << "[Thread average] Average value: " << data->avg << std::endl;
    return 0;
}