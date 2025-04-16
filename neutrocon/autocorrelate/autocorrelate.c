double autocorrelate_list(const IntList *list, int k) {
    if (!list || list->count <= k) return 0.0;

    double avg = average(*list);
    double var = variance_sample((IntList*)list);
    if (var == 0.0) return 0.0;

    double numerator = 0.0;
    for (int i = 0; i < list->count - k; i++) {
        numerator += (list->data[i] - avg) * (list->data[i + k] - avg);
    }

    return numerator / (var * (list->count - k));
}

double* autocorrelate_grid(const IntGrid *grid, int k) {
    if (!grid || grid->count == 0) return NULL;

    double *result = malloc(grid->count * sizeof(double));
    if (!result) return NULL;

    for (int i = 0; i < grid->count; i++) {
        result[i] = autocorrelate_list(&grid->data[i], k);
    }

    return result;
}