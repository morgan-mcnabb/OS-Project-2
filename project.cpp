////////////////////////////////////////////////////////////
//  Author: Morgan McNabb
//  File: project.cpp
//  Class: CSCI-4727-940 Operating Systems
//  Due Date: November 30, 2020
//  Start Date: November 11, 2020
//  Last Updated: November 13, 2020
///////////////////////////////////////////////////////////


#include <string.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <signal.h>
#include <pthread.h>
#include "Logger.h"
#include "SafeQueue.h"

#define NUM_SHRUBBERIES 23
#define NUM_KNIGHTS 10

// for sending messages from the knights back to arthur
const pthread_t CREATOR_ID = pthread_self();

// flag for arthur's dropship
sig_atomic_t interrupted = false;

// the queues for the knights and the historian
SafeQueue<shrubbery> knight_queue[NUM_KNIGHTS];
SafeQueue<message> historian_queue;


// function sigusr1_handler
// ----------------------------
// the function that Arthur will be enter when interrupted
// with a SIGUSR1 signal from a knight
//
// @param signum: the signal that was sent
// @param siginfo: the information from the sender of the signal
// @param ignore: ignore this
//
// @returns: nothing

void sigusr1_handler(int signum, siginfo_t *si, void* ignore)
{
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


// function: historian_function
// -----------------------------
// the function that the historian thread will enter upon creation.
// logs messages sent from the knights and arthur
//
// @param arg: initialization data for the pthread
//
// @return: nothing

void *historian_function(void *arg)
{
    // spawn logger
    Logger logger("log");
    
    std::cout << "I am the historian and I will be logging this session for you!" << std::endl;

    // message data
    message msg;
    std::string sender;

    // do until message receives a -2, then stop
    do
    {
        msg = historian_queue.dequeue();

        if(msg.from < 0)
            sender = "Arthur";
        else
            sender = "Knight " + std::to_string(msg.from + 1);

        logger.log_from_sender(sender, msg.payload);
        
    }while(msg.from != -2);

    logger.end_log_session();

    std::cout << "It's time for me to leave, I will leave this log with you!" << std::endl;
    pthread_exit(0);
}

// function: knight_function
// ------------------------------
// the function that all knight threads will enter upon their creation.
// receives shrubberies from arthur and judges them whether they are worthy
// of planting. keeps a running tally of planted shrubberies and then sends 
// that information to the historian.
//
// @param arg: the initialization data for the pthread
//
// @return: nothing

void *knight_function(void *arg)
{
    int planted_shrubberies = 0;
    long knight = (long) arg;
    int cost_threshold = rand() % 20 + 1;
    shrubbery shrub;
    message msg;

    // do until the index = -1 which indicates it is the last shrubbery
    do
    {
        shrub = knight_queue[knight].dequeue();

        if(shrub.nice && shrub.cost <= cost_threshold)
            planted_shrubberies++;

        // send signal to arthur once 100 shrubs are planted
        if(planted_shrubberies == 100)
            pthread_kill(CREATOR_ID, SIGUSR1);

    }while(shrub.index != -1);

    // send tally to historian and exit
    std::string payload;
    payload = "I have planted " + std::to_string(planted_shrubberies) + " shrubberies from this shipment!\0";
    msg.from = knight;
    strcpy(msg.payload, payload.c_str());
    historian_queue.enqueue(msg);

    pthread_exit(NULL);
}


// function: send_shrubberies
// ----------------------------
// this function takes care of sending shrubberies evenly
// to each of the knight threads
//
// @param shrubs: the array full of shrubberies
//
// @return: nothing

void send_shrubberies(shrubbery shrubs[])
{
    int i, knight_num = 0;
    while(!interrupted)
    {
        i = rand() % NUM_SHRUBBERIES;
        knight_queue[knight_num].enqueue(shrubs[i]);
        knight_num++;
        if(knight_num >= NUM_KNIGHTS)
            knight_num = 0;

    }
}


// function: send_message
// -------------------------
// this function takes care of sending the 
// "It can't be done!" message to all of the knight
// threads upon receipt of a SIGUSR1 signal
//
// @return: nothing

void send_message()
{
    // 0 = nice, 0 = cost, -1 = index
    // -1 signifies that this is the last shrubbery
    shrubbery shrub = {0, 0, -1};

    for(int i = 0; i < NUM_KNIGHTS; i++)
    {
        knight_queue[i].enqueue(shrub);
    }
}


// function: arthur_function
// --------------------------
// the function the main thread (arthur) will use to
// spawn the threads and handle all communication with
// the knight threads and historian
//
// @return: nothing

void arthur_function()
{
    // sigaction initialization
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = (void (*)(int, siginfo_t*, void*))sigusr1_handler;
    if(sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("Bad sigaction");
        exit(EXIT_FAILURE);
    }

    // pthread declaration
    pthread_t knights[NUM_KNIGHTS];
    pthread_t historian;
    
    // shrubbery declaration and initialization
    shrubbery shrubs[NUM_KNIGHTS];
    populate_shrubberies(shrubs);
    
    // create the historian thread with proper error handling
    int rc;
    rc = pthread_create(&historian, NULL, historian_function, NULL);
    if(rc)
    {
        std::cout << "There was an error creating the historian thread" << std::endl;
        exit(-1);
    }

    // spawn the knights with proper error handling
    for(int i = 0; i < NUM_KNIGHTS; i++)
    {

        rc = pthread_create(&knights[i], NULL, knight_function, (void*)i);

        if(rc)
        {
            std::cout << "There was an error creating knight " << i << std::endl;
            exit(-1);
        }
    }


    send_shrubberies(shrubs); 

    send_message();

    // wait for all knight threads to finish
    for(int i = 0; i < NUM_KNIGHTS; i++)
        pthread_join(knights[i], NULL); 
   
    // send the final message to the historian and wait for the 
    // historian to finish and exit
    message msg;
    msg.from = -2;
    strcpy(msg.payload, "The \"Ekke Ekke Ekke Ekke Ptang Zoo Boing!\" knights escaped!");
    historian_queue.enqueue(msg);

    pthread_join(historian, NULL);

    // exit
    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));
    arthur_function();
    return 0;
}

