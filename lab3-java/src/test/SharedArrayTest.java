package test;

import main.SharedArray;
import main.Result;

import org.junit.jupiter.api.Test;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import static org.junit.jupiter.api.Assertions.*;

public class SharedArrayTest {

    @Test
    void startsWithZeros() {
        SharedArray a = new SharedArray(5);
        int[] snap = a.snapshot();
        for (int v : snap) assertEquals(0, v);
    }

    @Test
    void marksCorrectly() {
        SharedArray a = new SharedArray(3);
        Result r1 = a.tryMark(1, 2);
        Result r2 = a.tryMark(1, 3);
        assertTrue(r1.isMarked());
        assertFalse(r2.isMarked());
        assertEquals(2, a.snapshot()[1]);
    }

    @Test
    void clearsById() {
        SharedArray a = new SharedArray(5);
        a.tryMark(0, 1);
        a.tryMark(2, 1);
        a.clearById(1);
        int[] snap = a.snapshot();
        assertEquals(0, snap[0]);
        assertEquals(0, snap[2]);
    }

    @Test
    void outOfBoundsMarkFails() {
        SharedArray a = new SharedArray(3);
        assertFalse(a.tryMark(10, 1).isMarked());
        assertFalse(a.tryMark(-1, 1).isMarked());
    }

    @Test
    void doubleClearSafe() {
        SharedArray a = new SharedArray(4);
        a.tryMark(1, 3);
        a.clearById(3);
        a.clearById(3);
        assertEquals(0, a.snapshot()[1]);
    }

    @Test
    void clearOnlyOwnMarks() {
        SharedArray a = new SharedArray(6);
        a.tryMark(0, 1);
        a.tryMark(1, 2);
        a.tryMark(2, 1);
        a.tryMark(3, 2);
        a.clearById(1);
        int[] snap = a.snapshot();
        
        assertAll(
            () -> assertEquals(0, snap[0]),
            () -> assertEquals(2, snap[1]),
            () -> assertEquals(0, snap[2]),
            () -> assertEquals(2, snap[3])
        );
    }

    @Test
    void heavyMultithreadStress() throws InterruptedException {
        SharedArray a = new SharedArray(100);
        int threadCount = 12;
        ExecutorService executor = Executors.newFixedThreadPool(threadCount);

        for (int id = 1; id <= threadCount; id++) {
            int finalId = id;
            executor.submit(() -> {
                for (int i = 0; i < 2000; i++) {
                    a.tryMark(i % a.size(), finalId);
                }
            });
        }

        executor.shutdown();
        assertTrue(executor.awaitTermination(10, TimeUnit.SECONDS));
        int[] snap = a.snapshot();
        assertEquals(100, snap.length);
        for (int v : snap) assertTrue(v >= 0);
    }
}