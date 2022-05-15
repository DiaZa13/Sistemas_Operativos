#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main() {
    clock_t start, end;
    start = clock();

    int pid = fork();
    end = clock();

    if(pid == 0) {
        printf("CHILD: %f\n", (double) (end - start) / CLOCKS_PER_SEC);
    } else {
        printf("PARENT: %f\n", (double) (end - start) / CLOCKS_PER_SEC);
    }

    return 0;
}