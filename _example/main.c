#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

char *A = malloc(6);
strcpy(A, "Hello");
int AB = 123;
char **ABC = malloc(3 * sizeof(char*));
ABC[0] = strdup("A");
ABC[1] = strdup("B");
ABC[2] = strdup("C");
int *ABCD = malloc(3 * sizeof(int));
ABCD[0] = 1;
ABCD[1] = 2;
ABCD[2] = 3;

void src_test2() {
	clock_t __step_start__, __step_end__;
	__step_start__ = clock();
	__step_end__ = clock();
	printf("[Info]: %.2fms elapsed (src\test2)", ((double)(__step_end__-__step_start__)/CLOCKS_PER_SEC) * 1000);
}

void main() {
	clock_t __flow_start__, __flow_end__;
	__flow_start__ = clock();
	src\test2();
	printf("[Info]: %s", A);
