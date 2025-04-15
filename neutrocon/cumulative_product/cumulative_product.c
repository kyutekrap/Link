IntList cumulative_product(int period, IntList mlist) {
    IntList result = {mlist.count, NULL};
    result.data = malloc(mlist.count * sizeof(int));

    for (int i = 0; i < mlist.count; i++) {
        if (i < period - 1) {
            result.data[i] = 0;
        } else {
            int product = mlist.data[i - (period - 1)];
            for (int j = i - (period - 2); j <= i; j++) {
                product *= mlist.data[j];
            }
            result.data[i] = product;
        }
    }

    return result;
}