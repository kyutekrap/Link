IntList cumulative_quotient(int period, IntList mlist) {
    IntList result = {mlist.count, NULL};
    result.data = malloc(mlist.count * sizeof(int));

    for (int i = 0; i < mlist.count; i++) {
        if (i < period - 1) {
            result.data[i] = 0;
        } else {
            int quotient = mlist.data[i - (period - 1)];
            for (int j = i - (period - 2); j <= i; j++) {
                if (mlist.data[j] == 0)
                    printf("Dividing by 0");
                else
                    quotient /= mlist.data[j];
            }
            result.data[i] = quotient;
        }
    }

    return result;
}