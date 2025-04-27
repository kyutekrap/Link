#include "./libs/neutrocon/main.h";
#include <string.h>
#include <stdio.h>
#include <time.h>

const char A[] = "Hello";
const int AB = -123;
const char *ABC[2] = {"A", "B", "C"};
const double ABCD[2] = {1.1, 2.2, 3.3};
const int Grid1[2][2] = {
	{1, 2, 3},
	{1, 2, 3},
	{1, 2, 3}
};

const int Data1[8] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

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
	printf("[Error]: %d", AB);
	__flow_end__ = clock(); printf("[Info]: %.2fms elapsed (main)", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000); return;
}