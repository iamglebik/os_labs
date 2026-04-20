#pragma once
#include <vector>
#include <mutex>

class SharedArray;

class MarkerManager {
public:
    MarkerManager(int size);
    ~MarkerManager();

    void launch(int count);
    void start_all();
    void wait_all_blocked();
    bool terminate(int id);
    void resume_all();
    bool any_alive() const;
    int alive_count() const;
    int find_first_alive() const;
    void join_all();
    void print() const;

private:
    void worker(int id);
    static void sleep_ms(int ms);

    struct Marker {
        std::thread th;
        std::mutex mtx;
        std::condition_variable cv;

        std::atomic<bool> stop{ false };
        std::atomic<bool> alive{ true };
        std::atomic<bool> waiting{ false };

        Marker() = default;
        Marker(const Marker&) = delete;
        Marker& operator=(const Marker&) = delete;
    };
private:
    std::shared_ptr<SharedArray> array_;
    std::vector<std::unique_ptr<Marker>> markers_;
    mutable std::mutex ctrl_mtx_;
    std::condition_variable ctrl_cv_;
    std::atomic<bool> started_{ false };
    std::mutex start_mtx_;
    std::condition_variable start_cv_;

    static std::mutex cout_mtx_;
};
