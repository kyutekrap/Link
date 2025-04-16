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