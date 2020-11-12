#include <string.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <pthread.h>
#include "Logger.h"
#include "SafeQueue.h"

#define NUM_SHRUBBERIES 10
#define NUM_KNIGHTS 10

const pthread_t CREATOR_ID = pthread_self();
bool interrupted = false;
SafeQueue<message> message_queue[NUM_KNIGHTS];
SafeQueue<shrubbery> shrubbery_queue[NUM_KNIGHTS];

void sigusr1_handler(int signum, siginfo_t *si, void* ignore)
{
    // debugging
    std::cout << "I AM HERE " << std::endl;
    interrupted = true;
}

// function: populate_shrubberies
//---------------------------------
// populates the array of shrubberies
//
// @param shrubberies: the declared but uninitialized array of shrubberies
//
// returns: nothing
void populate_shrubberies(shrubbery shrubberies[])
{
    for(int i = 0; i < NUM_SHRUBBERIES; i++)
    {
        shrubberies[i].nice = rand() % 2;
        shrubberies[i].cost = rand() % 20 + 1;
        shrubberies[i].index = i;
    } 
}

void *knight_function(void *arg)
{
    int planted_shrubberies = 0;
    long knight = (long) arg;
    int cost_threshold = rand() % 20 + 1;
    shrubbery shrub;
    message msg;
    while(!interrupted)
    {
        shrub = shrubbery_queue[knight].dequeue();
        if(shrub.nice && shrub.cost <= cost_threshold)
            planted_shrubberies++;
        if(planted_shrubberies == 1)
            pthread_kill(CREATOR_ID, SIGUSR1);

    }
    std::cout << "reading messages" << std::endl;
    msg = message_queue[knight].dequeue();
    std::cout << "From: " << msg.from << " Payload: " << msg.payload << std::endl;
    pthread_exit(NULL);
}

void send_shrubberies(shrubbery shrubs[])
{
    int i, j = 0;
    while(!interrupted)
    {
        shrubbery_queue[j].enqueue(shrubs[i]);
        j++;
        if(j >= NUM_KNIGHTS)
            j = 0;

        i++;
        if(i >= NUM_SHRUBBERIES)
            i = 0;
    }
}

void send_message()
{
    message msg;
    msg.from = -1;
    strcpy(msg.payload, "It can't be done!");
    for(int i = 0; i < NUM_KNIGHTS; i++)
    {
        message_queue[i].enqueue(msg);
    }
}

void arthur_function()
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = (void (*)(int, siginfo_t*, void*))sigusr1_handler;
    if(sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("Bad sigaction");
        exit(EXIT_FAILURE);
    }

    pthread_t knights[NUM_KNIGHTS];
    shrubbery shrubs[NUM_KNIGHTS];
    populate_shrubberies(shrubs);

    int rc;
    
    // spawn the knights
    for(int i = 0; i < NUM_KNIGHTS; i++)
    {
        std::cout << "Creating thread " << i << std::endl;
        rc = pthread_create(&knights[i], NULL, knight_function, (void*)i);

        if(rc)
        {
            std::cout << "There was an error creating thread " << i << std::endl;
            exit(-1);
        }
    }


    send_shrubberies(shrubs); 
    std::cout << "sending message to knights" << std::endl;

    send_message();
    std::cout << "waiting for threads" << std::endl;

    for(int i = 0; i < NUM_KNIGHTS; i++)
        pthread_join(knights[i], NULL); 
   
    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));
    arthur_function();
    return 0;
}

