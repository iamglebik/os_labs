package main;

import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.Phaser;

public class Main {
    private static final Scanner scanner = new Scanner(System.in);

    public static void main(String[] args) {
        System.out.println("=== Laboratory work #3: Thread Synchronization (Java) ===");
        System.out.println("Author: Siniakou Hleb, group 12\n");

        int size = readPositiveInt("Enter array size: ");
        SharedArray array = new SharedArray(size);
        System.out.printf("Array initialized with zeros. Size: %d%n", size);

        int count = readPositiveInt("Enter number of marker threads: ");
        System.out.printf("Starting %d marker threads...%n", count);

        // Phaser для синхронизации всех потоков + main
        Phaser phaser = new Phaser(1); // Главный поток регистрируется
        List<MarkerThread> markers = new ArrayList<>();

        for (int i = 1; i <= count; i++) {
            MarkerThread marker = new MarkerThread(i, array, phaser);
            markers.add(marker);
            marker.start();
        }

        // Сигнал на начало работы
        phaser.arriveAndAwaitAdvance();

        while (markers.stream().anyMatch(MarkerThread::isAlive)) {
            // Ждём, пока все живые потоки не заблокируются
            phaser.arriveAndAwaitAdvance();

            System.out.println("\nArray state BEFORE termination:");
            printArray(array);

            // Если остался один живой, завершаем его автоматически
            long aliveCount = markers.stream().filter(MarkerThread::isAlive).count();
            if (aliveCount == 1) {
                MarkerThread last = markers.stream()
                        .filter(MarkerThread::isAlive)
                        .findFirst().orElse(null);
                if (last != null) {
                    System.out.printf("Only one marker (%d) remains - terminating it automatically.%n", last.getId());
                    last.terminate();
                    System.out.println("\nArray state AFTER termination:");
                    printArray(array);
                    break;
                }
            }

            int id = readMarkerId(count);
            MarkerThread toTerminate = markers.get(id - 1);

            if (!toTerminate.isAlive()) {
                System.err.println("Invalid or already terminated marker. Try another id.");
                continue;
            }

            toTerminate.terminate();

            System.out.println("\nArray state AFTER termination:");
            printArray(array);

            if (markers.stream().noneMatch(MarkerThread::isAlive)) {
                break;
            }

            // Сигнал оставшимся потокам продолжить работу
            phaser.arriveAndAwaitAdvance();
        }

        System.out.println("\nAll marker threads finished.");
    }

    private static int readPositiveInt(String prompt) {
        while (true) {
            System.out.print(prompt);
            if (scanner.hasNextInt()) {
                int value = scanner.nextInt();
                if (value > 0) {
                    scanner.nextLine(); // consume newline
                    return value;
                }
            }
            scanner.nextLine(); // clear invalid input
            System.err.println("Error: enter a valid positive number.");
        }
    }

    private static int readMarkerId(int maxId) {
        while (true) {
            System.out.print("Enter marker id to terminate: ");
            if (scanner.hasNextInt()) {
                int value = scanner.nextInt();
                scanner.nextLine();
                if (value >= 1 && value <= maxId) {
                    return value;
                }
            }
            scanner.nextLine();
            System.err.printf("Marker id must be in range [1..%d].%n", maxId);
        }
    }

    private static void printArray(SharedArray array) {
        int[] snap = array.snapshot();
        for (int i = 0; i < snap.length; i++) {
            System.out.print(snap[i]);
            if (i < snap.length - 1) {
                System.out.print(" ");
            }
        }
        System.out.println();
    }
}
