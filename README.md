# lib.thread

A library of custom threading data structures, including stateful POSIX threads, mutexes, wait queues, and semaphores

**Currently Implemented Constructs**

- stateful POSIX threads
- semaphores (weak)
- thread barrier

**Upcoming Constructs**

- semaphores (strong)
- generic wait queue

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
