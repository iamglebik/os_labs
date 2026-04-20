#include "Array.h"
#include <algorithm>
#include <numeric>

Array::Array() : n(0), minVal(0), maxVal(0), avg(0.0) {}

Array::Array(const int* data, int size) : arr(), n(size), minVal(0), maxVal(0), avg(0.0) {
    if (size > 0 && data) {
        arr.assign(data, data + size);
    }
    else {
        n = 0;
    }
}

Array::Array(const std::vector<int>& v) : arr(v), n((int)v.size()), minVal(0), maxVal(0), avg(0.0) {}

void Array::calculateMinMax() {
    if (n <= 0) return;
    int currMin = arr[0];
    int currMax = arr[0];
    for (int i = 1; i < n; ++i) {
        if (arr[i] < currMin) currMin = arr[i];
        if (arr[i] > currMax) currMax = arr[i];
    }
    minVal = currMin;
    maxVal = currMax;
}

void Array::calculateAverage() {
    if (n <= 0) {
        avg = 0.0;
        return;
    }
    long long sum = 0;
    for (int i = 0; i < n; ++i) sum += arr[i];
    avg = static_cast<double>(sum) / static_cast<double>(n);
}

void Array::makeResArray(int roundedAvg) {
    for (int i = 0; i < n; ++i) {
        if (arr[i] == minVal || arr[i] == maxVal) {
            arr[i] = roundedAvg;
        }
    }
}

const int* Array::dataPtr() const {
    if (n == 0) return 0;
    return &arr[0];
}
