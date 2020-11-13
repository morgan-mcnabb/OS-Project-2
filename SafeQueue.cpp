//////////////////////////////////////////////
//  Author: Morgan McNabb
//  File: SafeQueue.cpp
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: November 30, 2020
//  Start Date: November 11, 2020
//  Last Updated: November 13, 2020
//////////////////////////////////////////////
#include "SafeQueue.h"
#include <iostream>


    template <class T>
SafeQueue<T>::SafeQueue()
{
    sem_init(&semaphore, 0, 0);
    pthread_mutex_init(&lock, NULL);
}
    
    template <class T>
void SafeQueue<T>::enqueue(T message)
{
    pthread_mutex_lock(&lock);

    communication_queue.push(message);

    pthread_mutex_unlock(&lock);
    sem_post(&semaphore);
}

    template <class T>
T SafeQueue<T>::dequeue()
{
    sem_wait(&semaphore);
    pthread_mutex_lock(&lock);
    
    T data = communication_queue.front();
    communication_queue.pop();

    pthread_mutex_unlock(&lock);

    return data;
}


template class SafeQueue<message>;
template class SafeQueue<shrubbery>;
