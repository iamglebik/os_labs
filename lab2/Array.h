#ifndef ARRAY_H
#define ARRAY_H
#include <vector>

struct Array {
    std::vector<int> arr;
    int n;
    int minVal;
    int maxVal;
    double avg;

    Array();
    Array(const int* data, int size);
    Array(const std::vector<int>& v);

    void calculateMinMax();
    void calculateAverage();

    void makeResArray(int roundedAvg);

    const int* dataPtr() const;
};

#endif
