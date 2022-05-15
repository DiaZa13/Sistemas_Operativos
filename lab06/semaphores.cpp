/*
* Created by Zaray on 5/13/2022.
* Management of a digital resource trough semaphores
*/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

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

void *simulate_work(int thread_id){
    int wait;
    wait = 1 + rand() % 5; /* random between 1 and 5 */
    /* simulating using the resource by wait time */
    sleep(wait);

    logbook<<"⚒ Work done"<<" → "<<thread_id<<endl;
    return nullptr;
}

/* Function to consume the resource */
void *consume_resource(void *args){
    int thread_id = reinterpret_cast<int>((int *) args);

    for(int i=0; i<ITERATIONS; i++){
        /* acquiring the resource */
        sem_wait(&resources);
        logbook<<"Starting iteration "<<i+1<<" → "<<thread_id<<endl;

        logbook<<"⚠ Resource taken"<<" → "<<thread_id<<endl;
        simulate_work(thread_id);

        /* released the resource */
        logbook<<"🆓 Resource released "<<" → "<<thread_id<<endl;
        /* return the resource */
        sem_post(&resources);
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
