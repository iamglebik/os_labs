#include <gtest/gtest.h>
#include "shared_array.h"

TEST(SharedArray, StartsWithZeros) {
    SharedArray a(5);
    auto snap = a.snapshot();
    for (int v : snap) {
        EXPECT_EQ(v, 0);
    }
}

TEST(SharedArray, MarksCorrectly) {
    SharedArray a(3);
    EXPECT_TRUE(a.try_mark(1, 2));
    EXPECT_FALSE(a.try_mark(1, 3));
    EXPECT_EQ(a.get(1), 2);
}

TEST(SharedArray, ClearsById) {
    SharedArray a(5);
    a.try_mark(0, 1);
    a.try_mark(2, 1);
    a.clear_by_id(1);
    EXPECT_EQ(a.get(0), 0);
    EXPECT_EQ(a.get(2), 0);
}

TEST(SharedArray, OutOfBoundsMarkFails) {
    SharedArray a(3);
    EXPECT_FALSE(a.try_mark(10, 1));
    EXPECT_FALSE(a.try_mark(-1, 1));
}

TEST(SharedArray, DoubleClearSafe) {
    SharedArray a(4);
    a.try_mark(1, 3);
    a.clear_by_id(3);
    a.clear_by_id(3);
    EXPECT_EQ(a.get(1), 0);
}

TEST(SharedArray, ClearOnlyOwnMarks) {
    SharedArray a(6);
    a.try_mark(0, 1);
    a.try_mark(1, 2);
    a.try_mark(2, 1);
    a.try_mark(3, 2);

    a.clear_by_id(1);

    EXPECT_EQ(a.get(0), 0);
    EXPECT_EQ(a.get(1), 2);
    EXPECT_EQ(a.get(2), 0);
    EXPECT_EQ(a.get(3), 2);
}

TEST(SharedArray, GetOutOfBoundsSafe) {
    SharedArray a(3);
    EXPECT_EQ(a.get(100), -1);
    EXPECT_EQ(a.get(-5), -1);
}

TEST(SharedArray, PartialClearWorks) {
    SharedArray a(5);
    a.try_mark(0, 1);
    a.try_mark(1, 1);
    a.try_mark(2, 2);

    a.clear_by_id(1);

    EXPECT_EQ(a.get(0), 0);
    EXPECT_EQ(a.get(1), 0);
    EXPECT_EQ(a.get(2), 2);
}

TEST(SharedArray, HeavyMultithreadStress) {
    SharedArray a(100);
    const int THREADS = 12;

    std::vector<std::thread> workers;

    for (int id = 1; id <= THREADS; ++id) {
        workers.emplace_back([&a, id]() {
            for (int i = 0; i < 2000; ++i) {
                int idx = i % a.size();
                a.try_mark(idx, id);
            }
            });
    }

    for (auto& t : workers) {
        t.join();
    }

    auto snap = a.snapshot();
    EXPECT_EQ(snap.size(), 100);

    for (int v : snap) {
        EXPECT_TRUE(v >= 0);
    }
}

TEST(SharedArray, ConcurrentMarkAndClear) {
    SharedArray a(50);

    std::thread writer([&]() {
        for (int i = 0; i < 1000; ++i)
            a.try_mark(i % 50, 1);
        });

    std::thread cleaner([&]() {
        for (int i = 0; i < 100; ++i)
            a.clear_by_id(1);
        });

    writer.join();
    cleaner.join();

    auto snap = a.snapshot();
    EXPECT_EQ(snap.size(), 50);
}

TEST(SharedArray, MarkAfterClearWorks) {
    SharedArray a(4);
    a.try_mark(0, 1);
    a.clear_by_id(1);

    EXPECT_TRUE(a.try_mark(0, 2));
    EXPECT_EQ(a.get(0), 2);
}
