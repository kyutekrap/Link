void src_test2() {
	clock_t __step_start__, __step_end__; __step_start__ = clock();
	printf("[Warning]: %s", A);
	__step_end__ = clock(); printf("[Info]: %.2fms elapsed (src\test2)", ((double)(__step_end__-__step_start__)/CLOCKS_PER_SEC) * 1000); return;
}