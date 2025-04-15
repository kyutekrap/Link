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