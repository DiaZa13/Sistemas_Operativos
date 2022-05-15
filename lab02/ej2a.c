#include<stdio.h>
#include<time.h>

int main(){
	/* Variables */
	clock_t a, b;
	int i;
	double delta_time;
	a = clock(); 

	for (i = 0; i < 1000000; i++){printf("Ciclo 1: %d\n", i);}
	for (i = 0; i < 1000000; i++){printf("Ciclo 2: %d\n", i);}
	for (i = 0; i < 1000000; i++){printf("Ciclo 3: %d\n", i);}

	b = clock();
	delta_time = (double)(b - a);
	printf("Detal time: %f\n", delta_time);
	return 0; 
}
