int add_one(IntList mlist) {
    int res=0;
    for (int i=0; i<mlist.count; i++) {
        res += mlist.data[i];
    }
    return res;
}

IntList add_all(IntGrid mgrid) {
    IntList res = {0, NULL};

    if (mgrid.count == 0)
        return res;

    int res_size = mgrid.data[0].count;
    res.data = malloc(res_size * sizeof(int));

    for (int i=0; i<res_size; i++) {
        int temp=0;
        for (int j=0; j<mgrid.count; j++) {
            temp += mgrid.data[j].data[i];
        }
        res.data[i] = temp;
    }

    return res;
}

double average(IntList mlist) {
    if (mlist.count == 0) return 0.0;

    double sum = 0;
    for (int i = 0; i < mlist.count; i++) {
        sum += mlist.data[i];
    }

    return sum / mlist.count;
}

int divide_one(IntList mlist) {
    int res=0;
    for (int i=0; i<mlist.count; i++) {
        if (mlist.data[i] == 0)
            printf("Dividing by 0");
        else
            res /= mlist.data[i];
    }
    return res;
}

IntList divide_all(IntGrid mgrid) {
    IntList res = {0, NULL};

    if (mgrid.count == 0)
        return res;

    int res_size = mgrid.data[0].count;
    res.data = malloc(res_size * sizeof(int));

    for (int i=0; i<res_size; i++) {
        int temp=0;
        for (int j=0; j<mgrid.count; j++) {
            if (mgrid.data[j].data[i])
                printf("Dividing by 0");
            else
                temp /= mgrid.data[j].data[i];
        }
        res.data[i] = temp;
    }

    return res;
}

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

int multiply_one(IntList mlist) {
    int res=0;
    for (int i=0; i<mlist.count; i++) {
        res *= mlist.data[i];
    }
    return res;
}

IntList multiply_all(IntGrid mgrid) {
    IntList res = {0, NULL};

    if (mgrid.count == 0)
        return res;

    int res_size = mgrid.data[0].count;
    res.data = malloc(res_size * sizeof(int));

    for (int i=0; i<res_size; i++) {
        int temp=0;
        for (int j=0; j<mgrid.count; j++) {
            temp *= mgrid.data[j].data[i];
        }
        res.data[i] = temp;
    }

    return res;
}

int subtract_one(IntList mlist) {
    int res=0;
    for (int i=0; i<mlist.count; i++) {
        res -= mlist.data[i];
    }
    return res;
}

IntList subtract_all(IntGrid mgrid) {
    IntList res = {0, NULL};

    if (mgrid.count == 0)
        return res;

    int res_size = mgrid.data[0].count;
    res.data = malloc(res_size * sizeof(int));

    for (int i=0; i<res_size; i++) {
        int temp=0;
        for (int j=0; j<mgrid.count; j++) {
            temp -= mgrid.data[j].data[i];
        }
        res.data[i] = temp;
    }

    return res;
}