#include<stdio.h>
#include<time.h>

int main(){
	/* Variables */
	clock_t a, b;
	pid_t child, gchild, ggchild;
	int i;
	double delta_time;
	a = clock();
	child = fork();

	if(child == 0){
		gchild = fork();
		if(gchild == 0){
			ggchild = fork();
			if(ggchild == 0){
				for(i = 0; i < 1000000; i++){printf("Ciclo 1: %d\n", i);}
			}else{
				for(i = 0; i < 1000000; i++){printf("Ciclo 2: %d\n", i);}
				wait(NULL);
			}
		}else{
			for(i = 0; i < 1000000; i++){printf("Ciclo 3: %d\n", i);}
			wait(NULL);
		}
	}else{
		wait(NULL);
		b = clock();
		delta_time = (double)(b - a);
		printf("Delta time: %f\n", delta_time);
	}

	return 0;
}
