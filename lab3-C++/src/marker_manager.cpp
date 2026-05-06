#include "marker_manager.h"
#include "shared_array.h"
#include <iostream>

static std::mutex cout_mtx;

void MarkerManager::sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

MarkerManager::MarkerManager(int size) : started_(false) {
    if (size <= 0) throw std::invalid_argument("Size must be > 0.");
    array_ = std::make_shared<SharedArray>(size);
}

MarkerManager::~MarkerManager() {
    join_all();
}

void MarkerManager::launch(int count) {
    markers_.clear();

    for (int i = 0; i < count; ++i) {
        markers_.push_back(std::make_unique<Marker>());
    }

    for (int i = 0; i < count; ++i) {
        markers_[i]->th = std::thread(&MarkerManager::worker, this, i + 1);
    }
}

void MarkerManager::start_all() {
    {
        std::lock_guard<std::mutex> lock(start_mtx_);
        started_.store(true);
    }
    start_cv_.notify_all();
}

void MarkerManager::worker(int id) {
    srand(static_cast<unsigned>(id));

    {
        std::unique_lock<std::mutex> lock(start_mtx_);
        start_cv_.wait(lock, [&] {
            return started_.load();
            });
    }

    const int array_size = static_cast<int>(array_->size());

    while (true) {
        if (markers_[id - 1]->stop.load()) {
            break;
        }

        int index = rand() % array_size;

        if (array_->get(index) == 0) {
            sleep_ms(5);
            array_->try_mark(index, id);
            sleep_ms(5);
            continue;
        }

        int marked = array_->count_by_id(id);

        {
            std::lock_guard<std::mutex> lock(cout_mtx);
            std::cout << "Marker " << id
                << " cannot mark, marked: " << marked
                << ", index: " << index << std::endl;
        }

        markers_[id - 1]->waiting.store(true);
        ctrl_cv_.notify_one();

        std::unique_lock<std::mutex> lock(markers_[id - 1]->mtx);
        markers_[id - 1]->cv.wait(lock, [&] {
            return markers_[id - 1]->stop.load() ||
                !markers_[id - 1]->waiting.load();
            });

        if (markers_[id - 1]->stop.load()) {
            break;
        }
    }

    array_->clear_by_id(id);
    markers_[id - 1]->alive.store(false);
    ctrl_cv_.notify_one();
}

void MarkerManager::wait_all_blocked() {
    std::unique_lock<std::mutex> lock(ctrl_mtx_);
    ctrl_cv_.wait(lock, [&] {
        int alive = 0;
        int waiting = 0;

        for (auto& m : markers_) {
            if (m->alive.load()) {
                ++alive;
                if (m->waiting.load()) {
                    ++waiting;
                }
            }
        }

        return alive > 0 && alive == waiting;
        });
}

bool MarkerManager::terminate(int id) {
    if (id < 1 || id > static_cast<int>(markers_.size())) {
        return false;
    }

    auto& m = markers_[id - 1];
    if (!m->alive.load()) {
        return false;
    }

    m->stop.store(true);
    m->waiting.store(false);
    m->cv.notify_one();

    if (m->th.joinable()) {
        m->th.join();
    }

    array_->clear_by_id(id);
    m->alive.store(false);
    ctrl_cv_.notify_one();

    return true;
}

void MarkerManager::resume_all() {
    for (auto& m : markers_) {
        if (m->alive.load() && m->waiting.load()) {
            m->waiting.store(false);
            m->cv.notify_one();
        }
    }
}

bool MarkerManager::any_alive() const {
    for (auto& m : markers_) {
        if (m->alive.load()) {
            return true;
        }
    }
    return false;
}

int MarkerManager::alive_count() const {
    int cnt = 0;
    for (auto& m : markers_) {
        if (m->alive.load()) {
            ++cnt;
        }
    }
    return cnt;
}

int MarkerManager::find_first_alive() const {
    for (size_t i = 0; i < markers_.size(); ++i) {
        if (markers_[i]->alive.load()) {
            return static_cast<int>(i) + 1;
        }
    }
    return -1;
}

void MarkerManager::join_all() {
    for (auto& m : markers_) {
        m->stop.store(true);
        m->waiting.store(false);
        m->cv.notify_one();
    }

    for (auto& m : markers_) {
        if (m->th.joinable()) m->th.join();
    }
}

void MarkerManager::print() const {
    auto snap = array_->snapshot();
    std::lock_guard<std::mutex> lock(cout_mtx);
    for (size_t i = 0; i < snap.size(); ++i) {
        std::cout << snap[i];
        if (i + 1 < snap.size()) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}