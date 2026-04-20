#pragma once
#include <vector>
#include <mutex>

class SharedArray {
public:
    SharedArray(int size);

    bool try_mark(int index, int id);
    void clear_by_id(int id);
    int count_by_id(int id) const;
    int get(int index) const;
    std::vector<int> snapshot() const;
    int size() const;

private:
    std::vector<int> data_;
    mutable std::mutex mtx_;
};