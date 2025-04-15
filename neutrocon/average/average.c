double average(IntList mlist) {
    if (mlist.count == 0) return 0.0;

    double sum = 0;
    for (int i = 0; i < mlist.count; i++) {
        sum += mlist.data[i];
    }

    return sum / mlist.count;
}