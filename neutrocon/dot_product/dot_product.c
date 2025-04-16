double dot_product_vectors(const IntList *x, const IntList *y) {
    if (!x || !y || x->count != y->count) return 0.0;
    
    double result = 0.0;
    for (int i = 0; i < x->count; i++) {
        result += x->data[i] * y->data[i];
    }
    return result;
}

IntGrid dot_product_matrixes(const IntGrid *A, const IntGrid *B) {
    if (!A || !B || A->count == 0 || B->count == 0 || A->data[0].count != B->count) {
        return (IntGrid){0, NULL};
    }

    int rows_A = A->count;
    int cols_A = A->data[0].count;
    int cols_B = B->data[0].count;

    IntGrid result = {rows_A, malloc(rows_A * sizeof(IntList))};
    if (!result.data) return (IntGrid){0, NULL};

    for (int i = 0; i < rows_A; i++) {
        result.data[i].count = cols_B;
        result.data[i].data = malloc(cols_B * sizeof(int));
        if (!result.data[i].data) return (IntGrid){0, NULL};

        for (int j = 0; j < cols_B; j++) {
            int sum = 0;
            for (int k = 0; k < cols_A; k++) {
                sum += A->data[i].data[k] * B->data[k].data[j];
            }
            result.data[i].data[j] = sum;
        }
    }

    return result;
}

IntList dot_product_matrix_vector(const IntGrid *A, const IntList *B) {
    if (!A || !B || A->data[0].count != B->count) {
        return (IntList){0, NULL};
    }

    IntList result = {A->count, malloc(A->count * sizeof(int))};
    if (!result.data) return (IntList){0, NULL};

    for (int i = 0; i < A->count; i++) {
        result.data[i] = dot_product_rows(&A->data[i], B);
    }

    return result;
}