#ifndef LIB_THREAD_H
#define LIB_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

/* Thread Flags */

/* Thread is running its routine as is typical */
#define THREAD_F_RUNNING (1 << 0)
/* Thread has been marked for suspension, but is not yet in a suspended state */
#define THREAD_F_SUSPEND_PENDING (1 << 1)
/* Thread is suspended */
#define THREAD_F_SUSPENDED (1 << 2)
/* Thread is blocked on its condition variable (and not suspended)*/
#define THREAD_F_BLOCKED (1 << 3)

/* Canonical Aliases */

#define P(sem) sem_wait(sem)
#define V(sem) sem_post(sem)

#define UP(sem) sem_wait(sem)
#define DOWN(sem) sem_post(sem)

/* Structures */

typedef struct semaphore {
  int permit_counter;
  pthread_cond_t cv;
  pthread_mutex_t mutex; /* Controls access to `permit_counter` */
} semaphore_t;

typedef struct barrier {
  uint32_t threshold;
  uint32_t curr_wait; /* from 0 -> threshold - 1*/
  pthread_cond_t cv;
  pthread_mutex_t mutex; /* perform mutually exclusive ops on a thread barrier */
  bool is_ready; /* is barrier disposition in progress? */
  pthread_cond_t busy_cv; /* thread barrier in disposition progress state, block inbound threads */
} barrier_t;

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
	void* resume_arg; /* Data passed to the thread resume routine */
  void* (*thread_resume_routine)(void*); /* The routine executed when the thread awakens from suspension */
	pthread_attr_t attrs; /* The pthread_t member's attributes */
  semaphore_t* semaphore; /* The pthread_t member's semaphore structure */
  pthread_cond_t cv; /* The pthread_t member's condition variable */
	pthread_mutex_t mutex; /* Update thread state in a mutually exclusive manner */
	uint32_t flags; /* Bitflags for tracking thread state */
} thread_t;

/* Weak Semaphore API */

semaphore_t* semaphore_init(semaphore_t* sem, int count);

semaphore_t* semaphore_wait(semaphore_t* sem);

semaphore_t* semaphore_post(semaphore_t* sem);

semaphore_t* semaphore_destroy(semaphore_t* sem);

int semaphore_reveal(semaphore_t* sem);

/* Thread Barrier API */
void barrier_init(barrier_t* barrier, uint32_t threshold);

void barrier_wait(barrier_t* barrier);

/* Stateful POSIX Thread API */

thread_t* thread_init(thread_t* thread, char* name);

void thread_run(thread_t* thread, void*(*thread_routine)(void*), void* arg);

thread_t* thread_set_attr(thread_t *thread, bool joinable);

/* Suspensions API */

void thread_suspend(thread_t* thread);

void thread_test_suspend(thread_t* thread);

void thread_resume(thread_t* thread);

void thread_set_suspend_routine(
	thread_t* thread,
	void*(*thread_routine)(void*),
	void* suspend_arg
);

#endif /* LIB_THREAD_H */
