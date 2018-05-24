#ifndef __THREAD_H__
#define __THREAD_H__

#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>

#include <cstdlib>

using namespace std;

class Thread {
public:
    Thread();
    ~Thread();
    int create(void *callback, void *args);
    int join();

    static int initMutex();
    static int lockMutex();
    static int unlockMutex();

public:
    pthread_t tid;

private:
    static pthread_mutex_t mutex;
};

#endif
