#include "libthread.h"

#include "util.h"

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
	if (!thread) thread = malloc(sizeof(thread_t));

	if (strlcpy(thread->name, name, sizeof(thread->name)) >= sizeof(thread->name)) {
		return NULL;
	}

	thread->thread_created = false;
	thread->thread_routine = NULL;
	thread->arg = NULL;
	thread->thread_resume_routine = NULL;
	thread->resume_arg = NULL;
	thread->flags = 0;

	pthread_mutex_init(&thread->mutex, NULL);
	pthread_cond_init(&thread->cv, 0);
	pthread_attr_init(&thread->attrs);

	return thread;
}

/**
 * @brief Create the thread and invoke its routine
 * Defaults to a joinable thread
 *
 * @param thread
 * @param thread_routine
 * @param arg
 *
 * @returns bool Was the function able to create the pthread?
 */
bool thread_run(thread_t* thread, void*(*thread_routine)(void*), void* arg) {
	thread->thread_routine = thread_routine;
	thread->arg = arg;

	if (pthread_create(
		&thread->thread,
		&thread->attrs,
		thread_routine,
		arg
	) != 0) {
		perror("thread creation");
		return false;
	}

	SET_BIT(thread->flags, THREAD_F_RUNNING);
	thread->thread_created = true;

	return true;
}

/**
 * @brief Change the thread's JOINABLE attribute
 *
 * @param thread
 * @param joinable
 * @return thread_t*
 */
thread_t* thread_set_attr(thread_t *thread, bool joinable) {
	pthread_attr_setdetachstate(&thread->attrs, joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);
}
