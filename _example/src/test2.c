void test2() {
	clock_t __step_start__, __step_end__;
	__step_start__ = clock();
	__step_end__ = clock();
	printf("[Info]: %.2fms elapsed (test2)", ((double)(__step_end__-__step_start__)/CLOCKS_PER_SEC) * 1000);
}