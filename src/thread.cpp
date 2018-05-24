#include "thread.h"

using namespace std;

pthread_mutex_t Thread::mutex;

Thread::Thread()
{}

Thread::~Thread()
{}

int Thread::create(void *callback, void *args)
{
    int rc = 0;

    rc = pthread_create(&this->tid, NULL, (void *(*)(void *))callback, args);
    if(rc) {
        cerr << "Failed to create thread" << endl;
        return rc;
    } else {
        cout << "Thread successfully created" << endl;
    }

    return 0;
}

int Thread::join()
{
    pthread_join(this->tid, NULL);
    return 0;
}

int Thread::initMutex()
{
    if(pthread_mutex_init(&Thread::mutex, NULL) < 0) {
        cerr << "Error while initializing mutex" << endl;
        return -1;
    } else {
        cout << "Mutex initialized" << endl;
    }

    return 0;
}

int Thread::lockMutex()
{
    if(pthread_mutex_lock(&Thread::mutex) != 0) {
        cerr << "Error while trying to acquire the lock" << endl;
        return -1;
    } else {
        cout << "Acquire the lock" << endl;
    }

    return 0;
}

int Thread::unlockMutex()
{
    if(pthread_mutex_unlock(&Thread::mutex) != 0) {
        cerr << "Error while trying to release the lock" << endl;
        return -1;
    } else {
        cout << "Release the lock" << endl;
    }

    return 0;
}
