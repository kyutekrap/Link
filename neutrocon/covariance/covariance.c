double covariance_sample(const IntList *xlist, const IntList *ylist) {
    if (!xlist || !ylist) return 0.0;
    int n = xlist->count;
    if (n <= 1 || ylist->count != n) return 0.0;

    double sum_x = 0.0, sum_y = 0.0;
    for (int i = 0; i < n; i++) {
        sum_x += xlist->data[i];
        sum_y += ylist->data[i];
    }

    double mean_x = sum_x / n;
    double mean_y = sum_y / n;

    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += (xlist->data[i] - mean_x) * (ylist->data[i] - mean_y);
    }

    return sum / (n - 1);
}

double covariance_population(const IntList *xlist, const IntList *ylist) {
    if (!xlist || !ylist) return 0.0;
    int n = xlist->count;
    if (n <= 1 || ylist->count != n) return 0.0;

    double sum_x = 0.0, sum_y = 0.0;
    for (int i = 0; i < n; i++) {
        sum_x += xlist->data[i];
        sum_y += ylist->data[i];
    }

    double mean_x = sum_x / n;
    double mean_y = sum_y / n;

    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += (xlist->data[i] - mean_x) * (ylist->data[i] - mean_y);
    }

    return sum / n;
}