IntList cumulative_difference(int period, IntList mlist) {
    IntList result = {mlist.count, NULL};
    result.data = malloc(mlist.count * sizeof(int));

    for (int i = 0; i < mlist.count; i++) {
        if (i < period - 1) {
            result.data[i] = 0;
        } else {
            int diff = mlist.data[i - (period - 1)];
            for (int j = i - (period - 2); j <= i; j++) {
                diff -= mlist.data[j];
            }
            result.data[i] = diff;
        }
    }

    return result;
}

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