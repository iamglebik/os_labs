package main;

import java.util.Random;
import java.util.concurrent.Phaser;
import java.util.concurrent.atomic.AtomicBoolean;

public class MarkerThread implements Runnable {
    private final int id;
    private final SharedArray array;
    private final Phaser phaser;
    private final AtomicBoolean stop = new AtomicBoolean(false);
    private final AtomicBoolean waiting = new AtomicBoolean(false);
    private final Object pauseLock = new Object();
    private Thread thread;
    private volatile boolean alive = true;

    public MarkerThread(int id, SharedArray array, Phaser phaser) {
        this.id = id;
        this.array = array;
        this.phaser = phaser;
        this.phaser.register();
    }

    public void start() {
        thread = new Thread(this);
        thread.start();
    }

    @Override
    public void run() {
        Random random = new Random(id);
        phaser.arriveAndAwaitAdvance(); 

        while (!stop.get()) {
            int index = random.nextInt(array.size());

            Result res = array.tryMark(index, id);

            if (res.isMarked()) {
                try { Thread.sleep(5); } catch (InterruptedException ignored) {}
                try { Thread.sleep(5); } catch (InterruptedException ignored) {}
            } else {
                System.out.printf("Marker %d cannot mark, marked: %d, index: %d%n",
                        id, res.getMarkedCount(), index);

                waiting.set(true);
                phaser.arriveAndAwaitAdvance();

                synchronized (pauseLock) {
                    waiting.set(false);
                }
            }
        }

        array.clearById(id);
        alive = false;
        phaser.arriveAndAwaitAdvance(); 
    }

    public void terminate() {
        stop.set(true);
        synchronized (pauseLock) {
            pauseLock.notifyAll();
        }
        try {
            thread.join();
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public boolean isAlive() {
        return alive && thread != null && thread.isAlive();
    }

    public boolean isWaiting() {
        return waiting.get();
    }

    public int getId() {
        return id;
    }
}