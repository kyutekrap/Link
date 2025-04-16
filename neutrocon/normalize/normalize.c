double* normalize_l2(const IntList *list) {
    if (!list || list->count == 0) return NULL;

    double sum_sq = 0.0;
    for (int i = 0; i < list->count; i++) {
        sum_sq += list->data[i] * list->data[i];
    }
    double norm = sqrt(sum_sq);

    double *result = malloc(list->count * sizeof(double));
    if (!result) return NULL;

    if (norm > 0) {
        for (int i = 0; i < list->count; i++) {
            result[i] = list->data[i] / norm;
        }
    } else {
        for (int i = 0; i < list->count; i++) {
            result[i] = list->data[i];
        }
    }

    return result;
}

DoubleGrid normalize_minmax(const IntGrid *grid) {
    int global_min = grid->data[0].data[0];
    int global_max = grid->data[0].data[0];

    for (int i = 0; i < grid->count; i++) {
        for (int j = 0; j < grid->data[i].count; j++) {
            int val = grid->data[i].data[j];
            if (val < global_min) global_min = val;
            if (val > global_max) global_max = val;
        }
    }

    DoubleGrid result;
    result.count = grid->count;
    result.data = malloc(grid->count * sizeof(DoubleList));

    for (int i = 0; i < grid->count; i++) {
        int row_len = grid->data[i].count;
        result.data[i].count = row_len;
        result.data[i].data = malloc(row_len * sizeof(double));

        for (int j = 0; j < row_len; j++) {
            int val = grid->data[i].data[j];
            if (global_max != global_min) {
                result.data[i].data[j] = (double)(val - global_min) / (global_max - global_min);
            } else {
                result.data[i].data[j] = 0.0;
            }
        }
    }

    return result;
}