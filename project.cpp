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

struct thread_data
{
    pthread_t creator_pid;
    int knight_num;
};

const pthread_t CREATOR_ID = pthread_self();
bool interrupted = false;
SafeQueue<message> message_queue[NUM_KNIGHTS];
SafeQueue<shrubbery> shrubbery_queue[NUM_KNIGHTS];

void sigusr1_handler(int signum, siginfo_t *si, void* ignore)
{
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

// function: knight_function
// -------------------------
// the function that all knight processes (3 in this case)
// will call. this function judges the shrubberies sent
// down the pipe and determines whether the shrubberies
// are worth planting.
//
// @param knight: the identifier of the knight that is using 
//                the function
//
// @returns: nothing
void knight_function(int knight)
{
    int planted_shrubberies = 0;
    // create the signal action that will change the behavior
    // when this process receives a SIGUSR2 signal
   // struct sigaction sa;
   // sa.sa_flags = SA_SIGINFO | SA_RESTART;
   // sigemptyset(&sa.sa_mask);
   // sa.sa_sigaction = (void (*)(int, siginfo_t*, void*)) knight_handler;
   // if(sigaction(SIGUSR2, &sa, NULL) == -1)
   // {
   //     perror("Bad sigaction.");
   //     exit(EXIT_FAILURE);
   // }
    
    // data to be used sending messages
   // shrubbery s;
   // message msg;
   // msg.from = knight;
   // std::string payload;
   // const int HISTORIAN_PIPE_WRITE = 2 * HISTORIAN + WRITE;

   // // data to be used for receiving messages
   // int cost_threshold = rand() % 10 + 1;
   // int pipe_index = 2*(knight);

    // while there is data in the pipe buffer

    // enqeue and dequeue messages here
}

//// function: historian_function
//// ----------------------------
//// receives messages from Arthur (parent process)
//// and the knight processes and logs them to a log
//// file
////
//// @param knight: the identifier for the historian
////
//// @returns: nothing
//void historian_function()
//{
//    // the historian will not be intentionally receiving any signals
//    // and if it does receive a signal, it will be handled by an empty
//    // method and just return from the interruption
//    struct sigaction sa;
//    sa.sa_flags = SA_SIGINFO | SA_RESTART;
//    sigemptyset(&sa.sa_mask);
//    sa.sa_sigaction = (void (*)(int, siginfo_t*, void*)) ignore_signal;
//    if(sigaction(SIGUSR2, &sa, NULL) == -1)
//    {
//        perror("Bad sigaction.");
//        exit(EXIT_FAILURE);
//    }
//
//    // for user interaction. the historian is the only process that prints to screen.
//    std::cout << "Hello! I am the historian and I will be logging this entire session for you!" << std::endl;
//
//    // begin logging
//    Logger logger("log");
//
//    // data for receiving messages
//    message msg;
//    const int HISTORIAN_PIPE_READ = 2 * HISTORIAN + READ;
//    std::string sender;
//    std::string payload;
//
//    // while there is data in the pipe buffer
//    while(read(fd[HISTORIAN_PIPE_READ], &msg, sizeof(message)) > 0)
//    {
//        if(msg.from == -1)
//            sender = "Arthur";
//        else if(strcmp(msg.payload, "") != 0)
//            sender = "Knight " + std::to_string(msg.from);
//
//        if(strcmp(msg.payload, "") !=  0)
//        {
//            // convert c_string to string and log the message
//            payload = msg.payload;
//            logger.log_from_sender(sender, payload);
//        }
//        else
//            planted_shrubberies += msg.from;
//    }
//
//    payload = "Total number of shrubs planted: " + std::to_string(planted_shrubberies);
//    logger.log(payload);
//
//    // tell user we are exiting, stop the logging session, and terminate self
//    std::cout << "Shew! Those knights are mean... I'm going to leave now. I will leave this log with you while I find another profession." << std::endl;
//    logger.end_log_session();
//    exit(0);
//}


void *knight_function(void *arg)
{
    int planted_shrubberies = 0;
    long knight = (long) arg;
    int cost_threshold = 20;
    std::cout << "Knight " << pthread_self() << std::endl;
    shrubbery shrub;
    message msg;
    while(msg.from != -1)
    {
        shrub = shrubbery_queue[knight].dequeue();
        if(shrub.nice && shrub.cost <= cost_threshold)
            planted_shrubberies++;
        if(planted_shrubberies == 1)
            pthread_kill(CREATOR_ID, SIGUSR1);

        std::cout << "reading messages" << std::endl;
        msg = message_queue[knight].dequeue();
        std::cout << "From: " << msg.from << " Payload: " << msg.payload << std::endl;
    }
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

void wait_for_threads(pthread_t knights[])
{
    for(int i = 0; i < NUM_KNIGHTS; i++)
        pthread_join(knights[i], NULL); 
    
    // write a message to the historian here
    // then wait for it to terminate too
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
    
    //pthread_create(&knights[NUM_KNIGHTS], NULL, historian_function, NULL);
    
    // spawn the knights
    for(int i = 0; i < NUM_KNIGHTS; i++)
    {
        std::cout << "Creating thread " << i << std::endl;
        pthread_create(&knights[i], NULL, knight_function, (void*)i);
    }


    send_shrubberies(shrubs); 
    std::cout << "sending message to knights" << std::endl;
    send_message();
    std::cout << "waiting for threads" << std::endl;
    wait_for_threads(knights);

   
   
    pthread_exit(NULL);
}



int main()
{
    srand(time(NULL));
    arthur_function();
    return 0;
}

