#include "./libs/neutrocon/main.h";
#include <string.h>
#include <stdio.h>
#include <time.h>

static char A[] = "Hello";
static double AB = -1.23;
static char *ABC[3] = {"A", "B", "C"};
static double ABCD[3] = {AB, 2.2, 3.3};
static double ABCDE[3];
memcpy(ABCDE, ABCD, sizeof(ABCD));
static int Grid1[3][3] = {
	{1, 2, 3},
	{1, 2, 3},
	{1, 2, 3}
};
static double Grid2[3][3] = {
	{ABCD},
	{ABCD},
	{ABCD}
};
static double Grid3[3][3] = {
	{1.1, 2.2, 3.3},
	{ABCD},
	{ABCD}
};

static int Data1[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

void src_test2() {
	clock_t __step_start__, __step_end__; __step_start__ = clock();
	printf("[Warning]: %s", A);
	__step_end__ = clock(); printf("[Info]: %.2fms elapsed (src\test2)", ((double)(__step_end__-__step_start__)/CLOCKS_PER_SEC) * 1000); return;
}

void main() {
	clock_t __flow_start__, __flow_end__; __flow_start__ = clock();
	static char A1[] = "Hello";
	static double AB1 = -1.23;
	static char *ABC1[3] = {"A", "B", "C"};
	static double ABCD1[3] = {AB, 2.2, 3.3};
	static int Grid11[3][3] = {
		{1, 2, 3},
		{1, 2, 3},
		{1, 2, 3}
	};
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
	printf("[Error]: %f", AB);
	__flow_end__ = clock(); printf("[Info]: %.2fms elapsed (main)", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000); return;
}