IntList rolling_product(int period, IntList mlist) {
    IntList result = {0, NULL};

    if (period <= 0 || mlist.count <= period) {
        return result;
    }

    int out_size = mlist.count - period;
    result.count = out_size;
    result.data = malloc(out_size * sizeof(int));

    for (int i = 0; i < out_size; i++) {
        result.data[i] = mlist.data[i + period] * mlist.data[i];
    }

    return result;
}