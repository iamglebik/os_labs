#include "Array.h"
#include "threads.h"
#include <gtest/gtest.h>
#include <windows.h>

void RunThreadAndWait(DWORD(*threadFunc)(LPVOID), Array* data) {
    HANDLE hThread = CreateThread(NULL, 0, threadFunc, data, 0, NULL);
    ASSERT_NE(hThread, (HANDLE)NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
}

TEST(ArrayTest, ReplaceMinElement) {
    int arr[] = { 1, 5, 10, 1 };
    Array data(arr, 4);
    data.minVal = 1; data.maxVal = 10; data.avg = 5.5;
    int avgRound = static_cast<int>(data.avg + 0.5);
    data.makeResArray(avgRound);
    EXPECT_EQ(data.arr[0], avgRound);
}

TEST(ArrayTest, ReplaceMaxElement) {
    int arr[] = { 1, 5, 10, 1 };
    Array data(arr, 4);
    data.minVal = 1; data.maxVal = 10; data.avg = 5.5;
    int avgRound = static_cast<int>(data.avg + 0.5);
    data.makeResArray(avgRound);
    EXPECT_EQ(data.arr[2], avgRound);
}

TEST(MinMaxThreadTest, FindCorrectMinMax) {
    int arr[] = { 7, 2, 9, 4 };
    Array data(arr, 4);
    RunThreadAndWait(MinMaxThread, &data);
    EXPECT_EQ(data.minVal, 2);
    EXPECT_EQ(data.maxVal, 9);
}

TEST(AverageThreadTest, CalculateCorrectAverage) {
    int arr[] = { 2, 4, 6, 8 };
    Array data(arr, 4);
    RunThreadAndWait(AverageThread, &data);
    EXPECT_DOUBLE_EQ(data.avg, 5.0);
}
TEST(ArrayTest, EmptyArray) {
    Array data;
    data.calculateMinMax();
    data.calculateAverage();
    EXPECT_EQ(data.n, 0);
    EXPECT_DOUBLE_EQ(data.avg, 0.0);
}

TEST(ArrayTest, SingleElementArray) {
    int arr[] = { 42 };
    Array data(arr, 1);
    data.calculateMinMax();
    data.calculateAverage();
    EXPECT_EQ(data.minVal, 42);
    EXPECT_EQ(data.maxVal, 42);
    EXPECT_DOUBLE_EQ(data.avg, 42.0);
}

TEST(ArrayTest, AllElementsEqual) {
    int arr[] = { 5, 5, 5, 5 };
    Array data(arr, 4);
    data.calculateMinMax();
    data.calculateAverage();
    int roundedAvg = static_cast<int>(data.avg + 0.5);
    data.makeResArray(roundedAvg);
    for (int i = 0; i < data.n; ++i)
        EXPECT_EQ(data.arr[i], 5);
}

TEST(ArrayTest, NegativeAndPositiveValues) {
    int arr[] = { -10, -5, 0, 5, 10 };
    Array data(arr, 5);
    RunThreadAndWait(MinMaxThread, &data);
    RunThreadAndWait(AverageThread, &data);
    EXPECT_EQ(data.minVal, -10);
    EXPECT_EQ(data.maxVal, 10);
    EXPECT_DOUBLE_EQ(data.avg, 0.0);
}

TEST(ArrayTest, ReplaceOnlyMinMax) {
    int arr[] = { 1, 2, 3, 4, 5 };
    Array data(arr, 5);
    data.minVal = 1;
    data.maxVal = 5;
    data.makeResArray(3);
    EXPECT_EQ(data.arr[0], 3);
    EXPECT_EQ(data.arr[4], 3);
    EXPECT_EQ(data.arr[2], 3); 
}

TEST(ThreadTest, MinMaxAndAverageTogether) {
    int arr[] = { 3, 6, 9, 12 };
    Array data(arr, 4);
    HANDLE h1 = CreateThread(NULL, 0, MinMaxThread, &data, 0, NULL);
    HANDLE h2 = CreateThread(NULL, 0, AverageThread, &data, 0, NULL);
    WaitForSingleObject(h1, INFINITE);
    WaitForSingleObject(h2, INFINITE);
    CloseHandle(h1);
    CloseHandle(h2);
    EXPECT_EQ(data.minVal, 3);
    EXPECT_EQ(data.maxVal, 12);
    EXPECT_DOUBLE_EQ(data.avg, 7.5);
}

TEST(ArrayTest, DataPointerNotNull) {
    int arr[] = { 1, 2, 3 };
    Array data(arr, 3);
    EXPECT_NE(data.dataPtr(), (const int*)0);
}

TEST(AverageTest, CorrectManualAverage) {
    int arr[] = { 1, 2, 3, 4, 5 };
    Array data(arr, 5);
    data.calculateAverage();
    EXPECT_DOUBLE_EQ(data.avg, 3.0);
}

TEST(ThreadTest, LongArrayAverage) {
    const int N = 100;
    std::vector<int> v(N, 10);
    Array data(v);
    RunThreadAndWait(AverageThread, &data);
    EXPECT_DOUBLE_EQ(data.avg, 10.0);
}