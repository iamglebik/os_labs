package main;

import java.util.Arrays;
import java.util.concurrent.locks.ReentrantLock;

public class SharedArray {
    private final int[] data;
    private final ReentrantLock lock = new ReentrantLock();

    public SharedArray(int size) {
        this.data = new int[size];
    }

    public Result tryMark(int index, int id) {
        lock.lock();
        try {
            if (index < 0 || index >= data.length) {
                return new Result(false, countById(id));
            }
            if (data[index] == 0) {
                data[index] = id;
                return new Result(true, countById(id));
            }
            return new Result(false, countById(id));
        } finally {
            lock.unlock();
        }
    }

    public void clearById(int id) {
        lock.lock();
        try {
            for (int i = 0; i < data.length; i++) {
                if (data[i] == id) {
                    data[i] = 0;
                }
            }
        } finally {
            lock.unlock();
        }
    }

    public int countById(int id) {
        lock.lock();
        try {
            int count = 0;
            for (int v : data) {
                if (v == id) count++;
            }
            return count;
        } finally {
            lock.unlock();
        }
    }

    public int[] snapshot() {
        lock.lock();
        try {
            return Arrays.copyOf(data, data.length);
        } finally {
            lock.unlock();
        }
    }

    public int size() {
        return data.length;
    }
}
