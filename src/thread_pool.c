#include "libthread.h"

#include <stdlib.h> /* for malloc */

/* Local Helpers Declarations */
static bool thread_pool_exec(thread_t* thread);
static void* thread_pool_resuspend(thread_pool_t* pool, thread_t* thread);
static void* thread_pool_exec_and_resuspend(void* arg);

thread_t* glued(glthread_t* glthreadptr) {
	return (thread_t*)((char*)(glthreadptr) - (char*)&(((thread_t*)0)->glthread));
}

/* Public API */

/**
 * @brief Initialize and allocate memory for a new thread pool
 *
 * @param pool
 */
void thread_pool_init(thread_pool_t* pool) {
	glthread_init(&pool->head);
	pthread_mutex_init(&pool->mutex, NULL);
}

/**
 * @brief Add a thread to the given pool in a mutally exclusive manner
 *
 * @param pool
 * @param thread
 */
void thread_pool_insert(thread_pool_t* pool, thread_t* thread) {
	pthread_mutex_lock(&pool->mutex);

	glthread_insert_after(&pool->head, &thread->glthread);

	pthread_mutex_unlock(&pool->mutex);
}

/**
 * @brief Get a thread for usage from the thread pool
 *
 * @param pool
 * @return thread_t*
 */
thread_t* thread_pool_get(thread_pool_t* pool) {
	thread_t* thread = NULL;
	glthread_t* glthread = NULL;

	pthread_mutex_lock(&pool->mutex);

	if (!(glthread = glthread_dequeue_first(&pool->head))) {
		pthread_mutex_unlock(&pool->mutex);
		return NULL;
	}

	thread = glued(glthread);

	pthread_mutex_unlock(&pool->mutex);

	return thread;
}

/**
 * @brief
 *
 * @param pool
 * @param thread_routing
 * @param routine_arg Typically application-specific data operated upon by the provided routine
 *
 * @return bool Indicates whether a thread was available
 */
bool thread_pool_dispatch(
	thread_pool_t* pool,
	void*(*thread_routine)(void*),
	void*routine_arg
) {
	thread_t* thread;

	if (!(thread = thread_pool_get(pool))) return false;

	thread_exec_data_t* thread_data = (thread_exec_data_t*)(thread->arg);

	if (!thread_data) {
		thread_data = malloc(sizeof(thread_exec_data_t));
	}

	thread_data->pool = pool;
	thread_data->thread = thread;

	thread_data->exec_arg = routine_arg;
	thread_data->exec_routine = thread_routine;
	thread_data->resuspend_routine = thread_pool_resuspend;
	thread->thread_routine = thread_pool_exec_and_resuspend;

	thread->arg = (void*)thread_data;

	// invoke thread fn now
	if (!thread_pool_exec(thread)) {
		return false;
	}

	return true;
}

/* Helpers */

/**
 * @brief Return the thread to the pool and suspend it
 *
 * Thread must be currently in a 'removed' state qua the thread pool
 * @param pool
 * @param thread
 */
static void* thread_pool_resuspend(thread_pool_t* pool, thread_t* thread) {
	pthread_mutex_lock(&pool->mutex);

	glthread_insert_after(&pool->head, &thread->glthread);

	if (thread->semaphore) {
		semaphore_post(thread->semaphore);
	}

	// suspend again
	pthread_cond_wait(&thread->cv, &thread->mutex);

	pthread_mutex_unlock(&pool->mutex);

	return NULL;
}

/**
 * @brief Execute the thread
 *
 * @param pool
 * @param thread
 *
 * @returns bool
 */
static bool thread_pool_exec(thread_t* thread) {
	// provided thread must be 'active' i.e. not in the thread pool currently
	if (!IS_GLTHREAD_EMPTY(&thread->glthread)) return false;

	// if the thread is already created, it is in a blocked state - thus, we resume it
	if (!thread->thread_created) {
		if (!thread_run(thread, thread->thread_routine, thread->arg)) {
			return false;
		}
	} else {
		pthread_cond_signal(&thread->cv);
	}

	return true;
}

/**
 * @brief Executes the execution and resuspension
 * stages of a thread pool thread
 *
 * @param arg
 * @return void*
 */
static void* thread_pool_exec_and_resuspend(void* arg) {
	thread_exec_data_t* thread_data = (thread_exec_data_t*)arg;

	while (1) {
		thread_data->exec_routine(thread_data->exec_arg);
		thread_data->resuspend_routine(thread_data->pool, thread_data->thread);
	}
}
