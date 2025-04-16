double variance_sample(IntList *list) {
    if (!list || list->count <= 1) return 0.0;

    int n = list->count;
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += list->data[i];
    }

    double mean = sum / n;

    double sum_squared_diff = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = list->data[i] - mean;
        sum_squared_diff += diff * diff;
    }

    return sum_squared_diff / (n - 1);
}

double variance_population(IntList *list) {
    if (!list || list->count <= 1) return 0.0;

    int n = list->count;
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += list->data[i];
    }

    double mean = sum / n;

    double sum_squared_diff = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = list->data[i] - mean;
        sum_squared_diff += diff * diff;
    }

    return sum_squared_diff / n;
}