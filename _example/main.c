#include <stdio.h>
#include <time.h>

char *A = malloc(6);
strcpy(A, "Hello");
int AB = 123;

void src_test2() {

}

void main() {
	clock_t __flow_start__, __flow_end__;
	__flow_start__ = clock();
	src_test2();
	printf("[Info]: %s", "Hello");
	printf("[Error]: %s", "Hello");
	printf("[Warning]: %s", "Hello");
	__flow_end__ = clock();
	printf("[Info]: %.2fms elapsed (main)", ((double)(__flow_end__-__flow_start__)/CLOCKS_PER_SEC) * 1000);
}