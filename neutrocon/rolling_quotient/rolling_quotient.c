IntList rolling_quotient(int period, IntList mlist) {
    IntList result = {0, NULL};

    if (period <= 0 || mlist.count <= period) {
        return result;
    }

    int out_size = mlist.count - period;
    result.count = out_size;
    result.data = malloc(out_size * sizeof(int));

    for (int i = 0; i < out_size; i++) {
        if (mlist.data[i])
            printf("Dividing by 0");
        else
            result.data[i] = mlist.data[i + period] / mlist.data[i];
    }

    return result;
}