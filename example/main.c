#include "libthread.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for sleep */

/**
 * @brief Print even numbers up to 10 in descending order
 *
 * @param arg
 * @return void*
 */
void* even_reverse_routine(void* arg) {
	for (int i = 10; i > 0; i -= 2) {
		printf("even n = %d\n", i);
		sleep(1);
	}

	return NULL;
}

/*
 * @brief Print odd numbers from 10 in descending order
 *
 * @param arg
 * @return void*
 */
void* odd_reverse_routine(void* arg) {
	for (int i = 9; i > 0; i -= 2) {
		printf("odd n = %d\n", i);
		sleep(1);
	}

	return NULL;
}

/**
 * @brief Print even numbers up to 10 in ascending order
 *
 * @param arg
 * @return void*
 */
void* even_routine(void* arg) {
	for (int i = 0; i < 10; i += 2) {
		printf("even n = %d\n", i);
		sleep(1);
	}

	return NULL;
}

/**
 * @brief Print odd numbers up to 10 in ascending order
 *
 * @param arg
 * @return void*
 */
void* odd_routine(void* arg) {
	for (int i = 1; i < 10; i += 2) {
		printf("odd n = %d\n", i);
		sleep(1);
	}

	return NULL;
}

int main(int argc, char* argv[]) {
	thread_pool_t* pool = malloc(sizeof(thread_pool_t));
	thread_pool_init(pool);

	thread_t* thread1;
	thread_t* thread2;

	if (!(thread1 = thread_init(thread1, "odd_thread"))) {
		return EXIT_FAILURE;
	}

	if (!(thread2 = thread_init(thread2, "even_thread"))) {
		return EXIT_FAILURE;
	}

	thread_pool_insert(pool, thread1);
	thread_pool_insert(pool, thread2);

	if (!thread_pool_dispatch(pool, odd_routine, 0)) {
		printf("failed dispatch\n");
	}

	if (!thread_pool_dispatch(pool, even_routine, 0)) {
		printf("failed dispatch\n");
	}

	sleep(20);

	printf("Dispatching with new work...\n");

	if (!thread_pool_dispatch(pool, odd_reverse_routine, 0)) {
		printf("failed dispatch\n");
	}


	if (!thread_pool_dispatch(pool, even_reverse_routine, 0)) {
		printf("failed dispatch\n");
	}

	pthread_exit(0);
	return EXIT_SUCCESS;
}
