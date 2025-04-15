int min(IntList mlist) {
    if (mlist.count == 0) return 0;

    int min_val = mlist.data[0];
    for (int i = 1; i < mlist.count; i++) {
        if (mlist.data[i] < min_val) {
            min_val = mlist.data[i];
        }
    }

    return min_val;
}