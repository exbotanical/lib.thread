#ifndef LIB_THREAD_H
#define LIB_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

/* Weak Semaphore */
#define P(sem) sem_wait(sem)
#define V(sem) sem_post(sem)

#define UP(sem) sem_wait(sem)
#define DOWN(sem) sem_post(sem)

typedef struct semaphore {
  int permit_counter;
  pthread_cond_t cv;
  pthread_mutex_t mutex; /* Controls access to `permit_counter` */
} semaphore_t;

semaphore_t* semaphore_init(semaphore_t* sem, int count);

semaphore_t* semaphore_wait(semaphore_t* sem);

semaphore_t* semaphore_post(semaphore_t* sem);

semaphore_t* semaphore_destroy(semaphore_t* sem);

int semaphore_reveal(semaphore_t* sem);

/* Thread Barrier */

typedef struct barrier {
  uint32_t threshold;
  uint32_t curr_wait; /* from 0 -> threshold - 1*/
  pthread_cond_t cv;
  pthread_mutex_t mutex; /* perform mutually exclusive ops on a thread barrier */
  bool is_ready; /* is barrier disposition in progress? */
  pthread_cond_t busy_cv; /* thread barrier in disposition progress state, block inbound threads */
} barrier_t;

void barrier_init(barrier_t* barrier, uint32_t threshold);

void barrier_wait(barrier_t* barrier);

/* Stateful POSIX Thread */

/**
 * @brief Describes the execution unit
 * data structure of a thread
 */
typedef struct thread {
  char name[32]; /* Name identifier for the thread */
  bool thread_created; /* Has the thread member been initialized? */
  pthread_t thread; /* Actual pthread_t */
  void* arg; /* Data passed to the thread routine */
  void* (*thread_routine)(void*); /* The routine executed upon thread creation */
  pthread_attr_t attrs; /* The pthread_t member's attributes */
  semaphore_t* semaphore; /* The pthread_t member's semaphore structure */
  pthread_cond_t cv; /* The pthread_t member's condition variable */
} thread_t;

thread_t* thread_init(thread_t* thread, char* name);

void thread_run(thread_t* thread, void*(*thread_routine)(void*), void* arg);

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

#endif /* LIB_THREAD_H */
