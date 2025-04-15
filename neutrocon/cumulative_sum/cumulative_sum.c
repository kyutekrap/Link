IntList cumulative_sum(int period, IntList mlist) {
    IntList result = {mlist.count, NULL};
    result.data = malloc(mlist.count * sizeof(int));

    for (int i = 0; i < mlist.count; i++) {
        if (i < period - 1) {
            result.data[i] = 0;
        } else {
            int sum = mlist.data[i - (period - 1)];
            for (int j = i - (period - 2); j <= i; j++) {
                sum += mlist.data[j];
            }
            result.data[i] = sum;
        }
    }

    return result;
}