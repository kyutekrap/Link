#include <time.h>
#include <stdio.h>

void test2() {
	clock_t __step_start__, __step_end__;
	__step_start__ = clock();
	__step_end__ = clock();
	printf("[Info] TAG took %f seconds\n", ((double)(__step_end__-__step_start__)) / CLOCKS_PER_SEC);
}