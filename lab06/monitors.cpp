/*
* Created by Zaray on 5/13/2022.
* Management of a digital resource trough monitors
*/

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include <cstdarg>
#include "monitors.h"

using namespace std;

// Adjustable parameters
#define THREADS 7
#define RESOURCES 5
#define ITERATIONS 2


FILE* logbook;
monitors monitor = monitors(RESOURCES);

void print_2_file(const char* format, ...){
    va_list args;
    va_start(args, format);
    vfprintf(logbook, format, args);
    va_end(args);
}


void *simulate_work(int thread_id){
    int wait =1 + rand() % 5; /* random between 1 and 5 */
    /* simulating using the resource by wait time */
    sleep(wait);

    print_2_file("⚒ Work done → %d\n", thread_id);
    return nullptr;
}

/* Function to consume the resource */
void *consume_resource(void *args){
    int thread_id = reinterpret_cast<int>((int *) args);
    int amount_resources = 1 + rand() % RESOURCES;
    print_2_file("🍴 Will be consume %d resources → %d\n", amount_resources, thread_id);
    for(int i=0; i<ITERATIONS; i++){
        print_2_file("Starting iteration %d → \n",i+1, thread_id);
        print_2_file("🖥 Mutex acquire, entering the monitor → %d\n",thread_id);
        /* acquiring the resource */
        if (!monitor.decrease_count(amount_resources))
            print_2_file("✅ Enough resources available... → %d\n", thread_id);
        else
            print_2_file("🚫 Not enough resources, it has to wait → %d\n", thread_id);
        monitor.acquire_resources(amount_resources);

        print_2_file("⚠ Resources taken: %d → %d\n", amount_resources, thread_id);
        simulate_work(thread_id);

        /* released the resource */
        print_2_file("🆓 Resources released: %d → %d\n", amount_resources, thread_id);
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
    logbook = fopen(argv[1],"w");
    if(logbook == nullptr){
        perror("Error opening the log");
    }

    pthread_t threads[THREADS]; /* Defines the array to save the id for the threads */
    int return_value;

    print_2_file("**STARTING SIMULATION**\n");
    print_2_file("Creating threads...\n");
    /* Creates the threads */
    for (int i=0; i<THREADS; i++){
        print_2_file("Initializing thread → %d\n",i+111);
        return_value = pthread_create(&threads[i], nullptr, consume_resource, (void *)(i+111));

        /* checking if the thread was created correctly */
        if (return_value){
            perror("Error creating the thread");
            return EXIT_FAILURE;
        }
    }

    /* main waits for all the threads to finish */
    print_2_file("Waiting for all threads to finish\n");
    pthread_exit(0);
}
