# lib.thread

A library of custom threading data structures, including stateful POSIX threads, mutexes, wait queues, and semaphores.

**Currently Implemented Constructs**

- stateful POSIX threads
- semaphores (weak)
- thread barrier
- suspension threads
- generic thread pool

**Upcoming Constructs**

- pausable thread
- semaphores (strong)
- generic wait queue

# Install

Via [clib](https://github.com/clibs/clib/):

```bash
clib install exbotanical/lib.thread
```

From Source:
```bash
git clone https://github.com/exbotanical/lib.thread
cd lib.thread && make
```

## Dynamic Linking

Linking to `lib.thread`:

```bash
# 1) include and use lib.thread in your project
# 2) generate object file for your project
gcc -I ../path/to/libthread -c main.c -o main.o
# 3) generate shared object file
make
# 4) link your project to lib.thread (and libbsd, also required here)
gcc -o main main.o -L../path/to/libthread -llibthread -lbsd
# you may need to add the lib location to your PATH
```

Linking to `lib.thread` on Windows:

```bash
# 1) include and use lib.thread in your project
# 2) generate object file for your project
gcc -I ../path/to/libthread -c main.c -o main.o
# 3) generate shared object file
make win
# 3) link your project to lib.thread
gcc -o main.exe main.o -L /path/to/lib.thread -llib_thread.dll
# you may need to add the lib location to your PATH
```

## Documentation and API

### Thread Barriers

A thread barrier is a thread synchronization data structure that blocks all threads at a specified critical section until a specified threshold of threads arrives at the barrier point.

For example, if your program has a section that must wait for a number of tasks to be completed before proceeding, you might use a thread barrier.

### Thread Pools

A thread pool is a data structure that holds suspended threads, each ready to resume and execute tasks.

Thread pools are useful because we are able to maintain initialized threads for the duration of a process without re-invoking `pthread_create`, an expensive wrapper for the `clone` system call.

Instead, threads are suspended after finishing their respective routines; we can call them back into the main thread's context and assign to them new work whenever we want to.

Find example usage of the thread pool API [here](./examples/thread_pool/)
