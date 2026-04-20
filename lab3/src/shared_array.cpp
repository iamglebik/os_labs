#include "shared_array.h"

SharedArray::SharedArray(int size) : data_(size, 0) {}

bool SharedArray::try_mark(int index, int id) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (data_[index] == 0) {
        data_[index] = id;
        return true;
    }
    return false;
}

void SharedArray::clear_by_id(int id) {
    std::lock_guard<std::mutex> lock(mtx_);
    for (int& v : data_) {
        if (v == id) {
            v = 0;
        }
    }
}

int SharedArray::count_by_id(int id) const {
    std::lock_guard<std::mutex> lock(mtx_);
    int c = 0;
    for (int v : data_) {
        if (v == id) {
            ++c;
        }
    }
    return c;
}

int SharedArray::get(int index) const {
    std::lock_guard<std::mutex> lock(mtx_);
    return data_[index];
}

std::vector<int> SharedArray::snapshot() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return data_;
}

int SharedArray::size() const {
    return data_.size();
}