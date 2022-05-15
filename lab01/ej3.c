#include<stdio.h>
#include<sys/syscall.h>

int main(){
	int x = syscall(345, 15);
	printf("La suma entre 13 y 15 es: %d\n",x);
	return(0);
}