#include <time.h>
#include <stdio.h>

void main() {
	clock_t __flow_start__, __flow_end__;
	__flow_start__ = clock();
	__flow_end__ = clock();
	printf("[Info] TAG took %f seconds\n", ((double)(__flow_end__-__flow_start__)) / CLOCKS_PER_SEC);
}