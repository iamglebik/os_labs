// ==================== main.cpp ====================
#include "marker_manager.h"
#include <iostream>

static int read_positive_int(const char* prompt) {
    int value = 0;
    while (true) {
        std::cout << prompt;
        if (!(std::cin >> value)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Error: enter a valid number.\n";
            continue;
        }
        if (value <= 0) {
            std::cerr << "Error: number must be > 0.\n";
            continue;
        }
        return value;
    }
}

static int read_marker_id(int max_id) {
    int id = 0;
    while (true) {
        std::cout << "Enter marker id to terminate: ";
        if (!(std::cin >> id)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Invalid input! Please enter a number.\n";
            continue;
        }
        if (id < 1 || id > max_id) {
            std::cerr << "Marker id must be in range [1.." << max_id << "].\n";
            continue;
        }
        return id;
    }
}

int main() {
    try {
        std::cout << "=== Laboratory work #3: Thread Synchronization ===\n";
        std::cout << "Author: Siniakou Hleb, group 12\n\n";

        int size = read_positive_int("Enter array size: ");
        MarkerManager manager(size);
        std::cout << "Array initialized with zeros. Size: " << size << "\n";

        int count = read_positive_int("Enter number of marker threads: ");
        std::cout << "Starting " << count << " marker threads...\n";
        manager.launch(count);
        manager.start_all();

        while (manager.any_alive()) {
            manager.wait_all_blocked();

            std::cout << "\nArray state BEFORE termination:\n";
            manager.print();

            if (manager.alive_count() == 1) {
                int last = manager.find_first_alive();
                if (last != -1) {
                    std::cout << "Only one marker (" << last << ") remains - terminating it automatically.\n";
                    manager.terminate(last);
                    std::cout << "\nArray state AFTER termination:\n";
                    manager.print();
                    break;
                }
            }

            int id = read_marker_id(count);

            if (!manager.terminate(id)) {
                std::cerr << "Invalid or already terminated marker. Try another id.\n";
                continue;
            }

            std::cout << "\nArray state AFTER termination:\n";
            manager.print();

            if (!manager.any_alive()) {
                break;
            }

            manager.resume_all();
        }

        manager.join_all();
        std::cout << "\nAll marker threads finished.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
        return 2;
    }
}