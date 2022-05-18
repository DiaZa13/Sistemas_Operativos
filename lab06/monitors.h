/*
 * Created by Zaray on 5/17/2022.
 * Defines a monitor to synchronize process
 * Based on: https://www.infor.uva.es/~fdiaz/aso/2004_05/doc/ASO_PR02_20041122.pdf
*/

#ifndef LAB6_MONITORS_H
#define LAB6_MONITORS_H
#include <pthread.h>


class monitors {

private:
    int available_resources;
    pthread_mutex_t mutex_resources;
    pthread_cond_t resources;

public:
    /* initializing the locks on the constructor of the class */
    explicit monitors(int initial_resources){
        available_resources = initial_resources;
        pthread_mutex_init(&mutex_resources, nullptr);
        pthread_cond_init(&resources, nullptr);
    }

    void acquire_resources(int requested_resources){
        pthread_mutex_lock(&mutex_resources);

        /* if there are not enough resources it has to wait*/
        /* while allows to check every time the process wakes up */
        while (decrease_count(requested_resources))
            pthread_cond_wait(&resources, &mutex_resources);

        available_resources -= requested_resources;
        /* unlock the mutex so other functions can enter  */
        pthread_mutex_unlock(&mutex_resources);
    }

    void release_resources(int requested_resources){
        pthread_mutex_lock(&mutex_resources);

        available_resources += requested_resources;
        /* after if released the used resources call signal to wake up other process */
        pthread_cond_signal(&resources);

        pthread_mutex_unlock(&mutex_resources);

    }

    /* Defines if it is able to acquire an amount of resources  */
    bool decrease_count(int amount) const{
        return (available_resources < amount);
    }

public:
    /* handles destroy the mutex and cond variables */
    ~monitors(){
        pthread_mutex_destroy(&mutex_resources);
        pthread_cond_destroy(&resources);
    }

};

#endif //LAB6_MONITORS_H
