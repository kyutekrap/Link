#include "./libs/list/main.h";
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

char *A = malloc(6);
strcpy(A, "Hello");
int AB = -123;
char **ABC = malloc(3 * sizeof(char*));
ABC[0] = strdup("A");
ABC[1] = strdup("B");
ABC[2] = strdup("C");
double *ABCD = malloc(3 * sizeof(double));
ABCD[0] = 1.1;
ABCD[1] = 2.2;
ABCD[2] = 3.3;
void **Map1 = malloc(2 * sizeof(void*));
Map1[0] = ABC;
Map1[1] = ABC;
void **Map2 = malloc(2 * sizeof(void*));
Map2[0] = ABC;
Map2[1] = malloc(3 * sizeof(int));
Map2[1][0] = 1;
Map2[1][1] = 2;
Map2[1][2] = 3;

int *Data1 = malloc(9 * sizeof(int));
Data1[0] = 1;
Data1[1] = 2;
Data1[2] = 3;
Data1[3] = 4;
Data1[4] = 5;
Data1[5] = 6;
Data1[6] = 7;
Data1[7] = 8;
Data1[8] = 9;

void src_test2() {
	clock_t __step_start__, __step_end__; __step_start__ = clock();
	printf("[Warning]: %s", A);
	__step_end__ = clock(); printf("[Info]: %.2fms elapsed (src\test2)", ((double)(__step_end__-__step_start__)/CLOCKS_PER_SEC) * 1000); return;
}

void main() {
	clock_t __flow_start__, __flow_end__; __flow_start__ = clock();
	if (strcmp(A, strdup("Hello")) == 0) { src_test2(); }
	if (AB > 0) { src_test2(); }
	else if (AB < 100) { __flow_end__ = clock(); printf("[Info]: %.2fms elapsed (main)", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000); return; }
	while (A > 0) {
		src_test2();
	}
	while (AB > 0) {
		src_test2();
		if (AB > 100) { __flow_end__ = clock(); printf("[Info]: %.2fms elapsed (main)", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000); return; }
	}
	printf("[Info]: %s", A);
	printf("[Error]: %s", AB);
	__flow_end__ = clock(); printf("[Info]: %.2fms elapsed (main)", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000); return;
}