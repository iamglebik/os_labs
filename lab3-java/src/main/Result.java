package main;

public class Result {
    private final boolean marked;
    private final int markedCount;

    public Result(boolean marked, int markedCount) {
        this.marked = marked;
        this.markedCount = markedCount;
    }

    public boolean isMarked() {
        return marked;
    }

    public int getMarkedCount() {
        return markedCount;
    }
}
