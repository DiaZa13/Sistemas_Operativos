#include <sys/time.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
// !!!!!! This value is CPU-dependent !!!!!!

//#define LOOP_ITERATIONS_PER_MILLISEC 155000

#define LOOP_ITERATIONS_PER_MILLISEC 178250
//#define LOOP_ITERATIONS_PER_MILLISEC 193750


#define MILLISEC 	1000
#define MICROSEC	1000000
#define NANOSEC 	1000000000



/*   This is how CASIO tasks are specified 

In the file linux-2.6.24-casio/include/linux/sched.h

also in the file /usr/include/bits/sched.h (the user space scheduler file)

struct sched_param {
	int sched_priority;
#ifdef CONFIG_SCHED_SMS_POLICY
	int casio_pid;
	unsigned long long deadline;
#endif
};
*/

double min_offset,max_offset; //seconds
double min_exec_time,max_exec_time; //seconds
double min_inter_arrival_time,max_inter_arrival_time; //seconds

unsigned int casio_id,jid=1;

/* Timer that eventually expire at some point in the future
 * itimerval → used to specify when a timer expires
 * members:
 * struct timeval it_interval → timer interval
 * struct timeval it_value → current value
 * timeval
 * members:
 * time_t tv_sec → seconds
 * suseconds_t tv_used → microseconds
 */
struct itimerval inter_arrival_time;

void burn_1millisecs() {
	unsigned long long i;
	for(i=0; i<LOOP_ITERATIONS_PER_MILLISEC; i++);
}

void burn_cpu(long milliseconds){
	long i;
	for(i=0; i<milliseconds; i++)
		burn_1millisecs();
}

/* Se encarga de limpiar los parámetros de calendarización de cada tarea
 * Es básicamente como hacer un reset
 * El valor del id=-1
 * El valor del deadline=0
 * */
void clear_sched_param(struct sched_param *param)
{
	param->casio_id=-1;
	param->deadline=0;
}
void print_task_param(struct sched_param *param)
{
    	printf("\npid[%d]\n",param->casio_id);
	printf("deadline[%llu]\n",param->deadline);
}

// Reset del temporizador
void clear_signal_timer(struct itimerval *t)
{
	t->it_interval.tv_sec = 0;
	t->it_interval.tv_usec = 0;
	t->it_value.tv_sec = 0;
	t->it_value.tv_usec = 0;
}

// Actualiza el valor actual del temporizador
void set_signal_timer(struct itimerval *t,double secs)
{
	t->it_interval.tv_sec = 0;
	t->it_interval.tv_usec =0 ;
	t->it_value.tv_sec = (int)secs;
	t->it_value.tv_usec = (secs-t->it_value.tv_sec)*MICROSEC; // es el valor de segundos ingresado convertido a micro
	
}
void print_signal_timer(struct itimerval *t)
{
	printf("Interval: secs [%ld] usecs [%ld] Value: secs [%ld] usecs [%ld]\n",
		t->it_interval.tv_sec,
		t->it_interval.tv_usec,
		t->it_value.tv_sec,
		t->it_value.tv_usec);
}

// Retorna de manera aleatorizada el tiempo
double get_time_value(double min, double max)
{
	if(min==max)
		return min;
	return (min + (((double)rand()/RAND_MAX)*(max-min)));
}

// Handler of personalized signal SIGUSR1
void start_task(int s)
{
	printf("\nTask(%d) has just started\n",casio_id);
    // Inicia el temporizador para la respectiva tarea
	set_signal_timer(&inter_arrival_time,get_time_value(min_offset,max_offset));
	setitimer(ITIMER_REAL, &inter_arrival_time, NULL);
}

// Hace cada uno de los jobs de la tarea respectiva
void do_work(int s)
{
    // sends a signal to the process when the timer ends
    // this allows to create a loop to process all the jobs on the current task
	signal(SIGALRM, do_work);
	set_signal_timer(&inter_arrival_time,get_time_value(min_inter_arrival_time,max_inter_arrival_time));
    // set the timer to ITIMER_REAL
	setitimer(ITIMER_REAL, &inter_arrival_time, NULL);

	clock_t start, end;
	double elapsed=0;
	start = clock();
	printf("Job(%d,%d) starts\n",casio_id,jid);
    // Simulates the usage of CPU
	burn_cpu(get_time_value(min_exec_time,max_exec_time)*MILLISEC);
	end = clock();
    // calculate the delta time that the job takes
	elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Job(%d,%d) ends (%f)\n",casio_id,jid,elapsed);
	jid++;

}

// Handler of signal SIGUSR2
void end_task(int s)
{
	printf("\nTask(%d) has finished\n",casio_id);
    // ends the process
	exit(0);
}

int main(int argc, char** argv) {

	struct sched_param param;
	
	unsigned long long seed;
	int i;

    // reset of the timers for every task to be executed
	clear_signal_timer(&inter_arrival_time);
	
	clear_sched_param(&param);

	param.sched_priority = 1;

    // sets the id of the task based on the arguments pass
	casio_id=param.casio_id=atoi(argv[1]);
	min_exec_time=atof(argv[2]);
	max_exec_time=atof(argv[3]);
	min_inter_arrival_time=atof(argv[4]);
	max_inter_arrival_time=atof(argv[5]);
	param.deadline=atof(argv[6])*NANOSEC;
	min_offset=atof(argv[7]);
	max_offset=atof(argv[8]);
	seed=atol(argv[9]);
	srand(seed);
	signal(SIGUSR1, start_task);
	signal(SIGALRM, do_work);
	signal(SIGUSR2, end_task);

	print_task_param(&param);
	printf("Before sched_setscheduler:priority %d\n",sched_getscheduler(0));

	if ( sched_setscheduler( 0, SCHED_CASIO , &param ) ==-1){
		perror("ERROR");
	}
	
	printf("After sched_setscheduler:priority %d\n",sched_getscheduler(0));

	//Wait for signals while the task is idle
	while(1){
		pause();	
	}

	return 0;
}
	
	
	

	
	
	
	

