#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>

int main(){
	pid_t a;
	int i;
	a = fork();
	if (a == 0){
		for(i = 0; i < 4000000; i++){
			printf("Child: %d\n", i);
		}
	}else{
		while(1){}
	}
	return 0;
}
