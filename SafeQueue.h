#ifndef __SAFEQUEUE_H
#define __SAFEQUEUE_H

#include <pthread.h>
#include <queue>
#include <semaphore.h>

// for sending data from a process to other processes
struct message 
{
    int from;           // 0 == Arthur, 1 == 1st knight, 2 == 2nd knight, 3 == 3rd knight
    char payload[64];   // cstyle string message (63 bytes, plus \0)
};

// for storing details related to shrubberies that 
// the knights will be judging
struct shrubbery
{
    bool nice;  // 0 == not nice; 1 == nice
    int cost;   // in pieces of gold
    int index;  // index in the array - for debugging and identification
};

    template <class T>
class SafeQueue
{
    private:
        std::queue<T> communication_queue;
        pthread_mutex_t lock;
        sem_t semaphore;

    public:
        SafeQueue();
        void enqueue(T message);
        T dequeue();
};

#endif
