
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define  BUF_LEN			200
#define  CASIO_TASKS_NUM	20


// Define la configuración de cada tarea con base en casio_task.c
struct casio_tasks_config {
	int pid;
	double min_exec;
	double max_exec;
	double min_inter_arrival;
	double max_inter_arrival;
	double deadline;
	double min_offset;
	double max_offset;
};

pid_t casio_tasks_pid [CASIO_TASKS_NUM]; // id del proceso que ejecuta la tarea
int casio_tasks_num=0; // cantidad de tareas a ser ejecutadas

// Obtiene el valor del intervalo para el temporizador
int get_int_val (char* str) 
{
    // Se encarga de leer únicamente el valor de duración y retorna el valor convertido a int
	char* s = str;
	int val;
	for (s = str;*s!='\t';s++);
	*s='\0';
	val=atoi(str);
	return val;
}

void print_casio_tasks_config(struct casio_tasks_config *tasks, int num)
{
	int i;
	printf("\nCASIO TASKS CONFIG\n");
	printf("pid\tmin_c\tmax_c\tmin_t\tmax_t\tdeadl\tmin_o\tmax_o\n");
	for(i=0;i<num;i++){
		printf("%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
		tasks[i].pid,
		tasks[i].min_exec,
		tasks[i].max_exec,
		tasks[i].min_inter_arrival,
		tasks[i].max_inter_arrival,
		tasks[i].deadline,
		tasks[i].min_offset,
		tasks[i].max_offset
		);
	}
}

// Inicializa todos los valores de los parámetros que definen una tarea
void clear_casio_tasks_config_info(struct casio_tasks_config *tasks, int num)
{
	int i;
	
	for(i=0;i<num;i++){
		tasks[i].pid=0;
		tasks[i].min_exec=0;
		tasks[i].max_exec=0;
		tasks[i].min_inter_arrival=0;
		tasks[i].max_inter_arrival=0;
		tasks[i].deadline=0;
		tasks[i].min_offset=0;
		tasks[i].max_offset=0;
	}
}

// Asigna a la estructura *tasks toda la información de configuración de las tareas a ejecutar
void get_casio_task_config_info(char * str, struct casio_tasks_config *tasks,int *n)
{
	char *s ,*s1;
	int i=0;
	s = s1=str;
	while(i<7){
		if(*s=='\t'){
			*s='\0';
			switch(i){
                // Cada uno de estos cases representa cada columna en el archivo system
				case 0:
                    // atoi se utiliza para convertir de string a integer
					tasks[*n].pid = atoi(s1);
					s1=s+1; // Indexado a través de la estructura que contiene la información de la tarea
					i++;
				break;
				case 1:
                    // atof se utiliza para convertir de string a double
					tasks[*n].min_exec = atof(s1);
					s1=s+1;
					i++;
				break;
				case 2:
					tasks[*n].max_exec = atof(s1);
					s1=s+1;
					i++;
				break;
				case 3:
					tasks[*n].min_inter_arrival = atof(s1);
					s1=s+1;
					i++;
				break;
				case 4:
					tasks[*n].max_inter_arrival = atof(s1);
					s1=s+1;
					i++;
				break;
				case 5:
					tasks[*n].deadline = atof(s1);
					s1=s+1;
					i++;
				break;
				case 6:
					tasks[*n].min_offset = atof(s1);
					s1=s+1;
					i++;
					tasks[*n].max_offset = atof(s1);
				break;

			}
			
		}
		s++; // Indexado en la información de configuración de cada tarea
	}
	(*n)++;	// Contador de las tareas, permite hacer el indexado en el puntero tasks
}

// Obtiene la información de configuración de las tareas del archivo system
void get_casio_tasks_config_info(char *file, int *duration, struct casio_tasks_config *tasks,int *n)
{
	char buffer[BUF_LEN];
	int count=0;
	FILE* fd  = fopen(file, "r");
	*n=0;
	buffer[0]='\0';
	while( (fgets(buffer, BUF_LEN, fd))!=NULL) {
		if(strlen(buffer)>1){
			switch(count){
                // AL leer la línea 0 toma el intervalo de duración de cada tarea
				case 0:
                    // actualiza el valor de duration por el valor leído en el archivo
					*duration=get_int_val(buffer);
					// actualiza el contador para pasar a la siguiente línea del archivo
                    count++;
				break;
				default:
                    // EL resto de líneas definen el resto de configuración para una tarea casio
                    // Va almacenando la información leída por el archivo system a un puntero que almacena todas las tareas
					get_casio_task_config_info(buffer, tasks,n);
			}
		}
		buffer[0]='\0';
	}
	fclose(fd);
	
}

void start_simulation()
{
	int i;
	printf("I will send a SIGUSR1 signal to start all tasks\n");
	for(i=0;i<casio_tasks_num;i++){
		kill(casio_tasks_pid[i],SIGUSR1);
	}
	
}

void end_simulation(int signal)
{
	int i;
	printf("I will send a SIGUSR2 signal to finish all tasks\n");
	for(i=0;i<casio_tasks_num;i++){
		kill(casio_tasks_pid[i],SIGUSR2);
	}
	
}

void help(char* name)
{
	fprintf(stderr, "Usage: %s file_name (system configuration)\n",	name);
	exit(0);
}


int main(int argc, char *argv[])
{
	
	int duration,i,j,k,n;
	struct casio_tasks_config casio_tasks_config[CASIO_TASKS_NUM];
	struct itimerval sim_time;// define el timer de la simulación
	char arg[CASIO_TASKS_NUM][BUF_LEN],*parg[CASIO_TASKS_NUM];
	
	srand(time(NULL));

	if(argc!=2){
		help(argv[0]);
	}

    // resettea las configuraciones que existan en el puntero de tareas
	clear_casio_tasks_config_info(casio_tasks_config, CASIO_TASKS_NUM);
	
    // obtiene la información de configuraicón de las tareas del archivo system
	get_casio_tasks_config_info(argv[1],&duration,casio_tasks_config,&casio_tasks_num);

    // define los valores del temporizador de la simualción
    // No se define un valor de expiración, pero si un valor de repetición el cual es la duración de cada tarea
	sim_time.it_interval.tv_sec = 0;
	sim_time.it_interval.tv_usec = 0;
	sim_time.it_value.tv_sec = duration;
	sim_time.it_value.tv_usec = 0;

    // Espera la señal del temporitzador para finalizar la simulación
	signal(SIGALRM, end_simulation);
	setitimer(ITIMER_REAL, &sim_time, NULL);	

    // Escribe en el documento la información de las tareas así como los resultados de la simulación
	for(i=0;i<casio_tasks_num;i++){
		strcpy(arg[0],"casio_task");	
				
		sprintf(arg[1],"%d",casio_tasks_config[i].pid);				
		sprintf(arg[2],"%f",casio_tasks_config[i].min_exec);			
		sprintf(arg[3],"%f",casio_tasks_config[i].max_exec);			
		sprintf(arg[4],"%f",casio_tasks_config[i].min_inter_arrival);			
		sprintf(arg[5],"%f",casio_tasks_config[i].max_inter_arrival);
		sprintf(arg[6],"%f",casio_tasks_config[i].deadline);	
		sprintf(arg[7],"%f",casio_tasks_config[i].min_offset);		
		sprintf(arg[8],"%f",casio_tasks_config[i].max_offset);	
		sprintf(arg[9],"%ld",rand());			
		n=10;
		for(k=0;k<n;k++){
			//printf("%s  ",arg[k]);
			parg[k]=arg[k];
		}
		parg[k]=NULL;
		
		// Crea otro proceso para ejecutar casio_task e imprimir los resultados en el archivo final
        casio_tasks_pid[i]=fork();

		if(casio_tasks_pid[i]==0){
            // Ejecuta el archivo casio_task y le pasa los parámetros definidos en el for anterior
			execv("./casio_task",parg);
			perror("Error: execv\n");
			exit(0);
		}		
		sleep(1);
	}
	
	start_simulation();  //time zero of the execution
	//waiting for the end of the simulation
	pause();

	for(i=0;i<casio_tasks_num;i++){
		wait(NULL);
	}

	printf("All tasks have finished properly!!!\n");
	return 0;	

}
