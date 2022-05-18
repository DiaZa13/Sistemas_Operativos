/*
* Created by Zaray on 5/13/2022.
* Management of a digital resource trough monitors
*/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "monitors.h"

using namespace std;

// Adjustable parameters
#define THREADS 7
#define RESOURCES 5
#define ITERATIONS 2

#define RESET "\e[0;0m"
#define CONSUME "\e[0;38;5;167m"
#define USE "\e[0;38;5;64m"
#define FREE "\e[0;38;5;208m"

sem_t resources;
ofstream logbook;
monitors monitor = monitors(RESOURCES);

void *simulate_work(int thread_id){
    int wait =1 + rand() % 5; /* random between 1 and 5 */
    /* simulating using the resource by wait time */
    sleep(wait);

    logbook<<"⚒ Work done"<<" → "<<thread_id<<endl;
    return nullptr;
}

/* Function to consume the resource */
void *consume_resource(void *args){
    int thread_id = reinterpret_cast<int>((int *) args);
    int amount_resources = 1 + rand() % RESOURCES;
    logbook<<"🍴 Will be consume "<<amount_resources<<"resources → "<<thread_id<<endl;
    for(int i=0; i<ITERATIONS; i++){
        logbook<<"Starting iteration "<<i+1<<" → "<<thread_id<<endl;
        logbook<<"🖥 Mutex acquire, entering the monitor → "<<thread_id<<endl;
        /* acquiring the resource */
        if (!monitor.decrease_count(amount_resources))
            logbook<<"✅ Enough resources available... → "<<thread_id<<endl;
        else
            logbook<<"🚫 Not enough resources, it has to wait → "<<thread_id<<endl;
        monitor.acquire_resources(amount_resources);

        logbook<<"⚠ Resources taken: "<<amount_resources<<" → "<<thread_id<<endl;
        simulate_work(thread_id);

        /* released the resource */
        logbook<<"🆓 Resources released: "<<amount_resources<<" → "<<thread_id<<endl;
        /* return the resource */
        monitor.release_resources(amount_resources);
    }

    pthread_exit(nullptr); /* after use, terminating the thread */
}

int main(int argc, char* argv[]){
    if(argc != 2) {
        perror("Usage: ./client <log.txt>");
        return EXIT_FAILURE;
    }

    /* txt file to save all the resources management */
    logbook.open(argv[1], ios::out);
    if(!logbook.is_open()){
        perror("Error opening the log");
    }

    pthread_t threads[THREADS]; /* Defines the array to save the id for the threads */
    int return_value;
    /* initializing the counting semaphore */
    return_value = sem_init(&resources, 0, RESOURCES);
    if (return_value){
        perror("Error creating the semaphore");
    }
    logbook<<"**STARTING SIMULATION**"<<endl;
    logbook<<"Creating threads..."<<endl;
    /* Creates the threads */
    for (int i=0; i<THREADS; i++){
        logbook<<"Initializing thread → "<<i+111<<endl;
        return_value = pthread_create(&threads[i], nullptr, consume_resource, (void *)(i+111));

        /* checking if the thread was created correctly */
        if (return_value){
            perror("Error creating the thread");
            return EXIT_FAILURE;
        }
    }

    /* main waits for all the threads to finish */
    logbook<<"Waiting for all threads to finish"<<endl;
    pthread_exit(0);
}
