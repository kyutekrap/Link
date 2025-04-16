double correlation_coefficient(IntList *xlist, IntList *ylist) {
    if (!xlist || !ylist) return 0.0;
    int n = xlist->count;
    if (n <= 1 || ylist->count != n) return 0.0;

    double sum_x = 0.0, sum_y = 0.0;
    double sum_xy = 0.0, sum_x2 = 0.0, sum_y2 = 0.0;

    for (int i = 0; i < n; i++) {
        double x = xlist->data[i];
        double y = ylist->data[i];
        sum_x += x;
        sum_y += y;
        sum_xy += x * y;
        sum_x2 += x * x;
        sum_y2 += y * y;
    }

    double numerator = (n * sum_xy) - (sum_x * sum_y);
    double denominator = sqrt((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y));

    if (denominator == 0.0) return 0.0;

    return numerator / denominator;
}