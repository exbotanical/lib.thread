#include "libthread.h"

#include <bsd/bsd.h>
#include <stdlib.h> /* for calloc, et al */
#include <bsd/string.h> /* for strlcpy */

/**
 * @brief Initialize the thread, allocating memory if it is NULL
 * Returns NULL upon failure to initialize
 *
 * @param thread
 * @param name
 * @return thread_t*
 */
thread_t* thread_init(thread_t* thread, char* name) {
	if (!thread) thread = calloc(1, sizeof(thread));

	if (strlcpy(thread->name, name, sizeof(thread->name)) >= sizeof(thread->name)) {
		return NULL;
	}

	thread->thread_created = false;
	thread->arg = NULL;
	thread->semaphore = NULL;
	thread->thread_routine = NULL;

	pthread_cond_init(&thread->cv, 0);
	pthread_attr_init(&thread->attrs);

	return thread;
}

/**
 * @brief Create the thread and invoke its routine
 *	Defaults to a joinable thread
 *
 * @param thread
 * @param thread_routine
 * @param arg
 */
void thread_run(thread_t* thread, void*(*thread_routine)(void*), void* arg) {
	thread->thread_routine = thread_routine;
	thread->arg = arg;

	pthread_attr_init(&thread->attrs);
	pthread_attr_setdetachstate(&thread->attrs, PTHREAD_CREATE_JOINABLE);

	if (pthread_create(
		&thread->thread,
		&thread->attrs,
		thread_routine,
		arg
	) != 0) {
		return NULL;
	}

	thread->thread_created = true;
}

thread_t* thread_set_attr(thread_t *thread, bool joinable);
