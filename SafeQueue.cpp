#include "SafeQueue.h"


    template <class T>
SafeQueue<T>::SafeQueue()
{
    sem_init(&semaphore, 0, 0);
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);
}

    template <class T> 
void SafeQueue<T>::enqueue(T message)
{
    pthread_mutex_lock(&lock);

    communication_queue.push(message);

    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&lock);
}

    template <class T>
T SafeQueue<T>::dequeue()
{
    pthread_mutex_lock(&lock);

    while(communication_queue.empty())
        pthread_cond_wait(&cond, &lock);


    T dequeued_message = communication_queue.front();
    communication_queue.pop();

    pthread_mutex_unlock(&lock);

    return dequeued_message;
}

template class SafeQueue<message>;
template class SafeQueue<shrubbery>;
