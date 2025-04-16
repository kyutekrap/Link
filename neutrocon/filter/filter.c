IntGrid low_pass_filter(const IntGrid *grid) {
    IntGrid result;
    result.count = grid->count;
    result.data = malloc(grid->count * sizeof(IntList));

    for (int i = 0; i < grid->count; i++) {
        int row_len = grid->data[i].count;
        result.data[i].count = row_len;
        result.data[i].data = malloc(row_len * sizeof(int));

        for (int j = 0; j < row_len; j++) {
            int sum = 0;
            int count = 0;

            for (int di = -1; di <= 1; di++) {
                int ni = i + di;
                if (ni < 0 || ni >= grid->count) continue;

                for (int dj = -1; dj <= 1; dj++) {
                    int nj = j + dj;
                    if (nj < 0 || nj >= grid->data[ni].count) continue;

                    sum += grid->data[ni].data[nj];
                    count++;
                }
            }

            result.data[i].data[j] = sum / count;
        }
    }

    return result;
}