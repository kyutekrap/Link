int max(IntList mlist) {
    if (mlist.count == 0) return 0;

    int max_val = mlist.data[0];
    for (int i = 1; i < mlist.count; i++) {
        if (mlist.data[i] > max_val) {
            max_val = mlist.data[i];
        }
    }

    return max_val;
}