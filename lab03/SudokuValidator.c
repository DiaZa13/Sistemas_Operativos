#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <syscall.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <omp.h>

#define SUDOKU_SIZE 9
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define NRM  "\x1B[0m"

/* global variables */
int values[9][9];

struct validation{
    bool column[9];
    bool row[9];
    bool array[9];
};

bool checkRow(int row){
    int column, value, validate = 0;
    for (column = 0; column < SUDOKU_SIZE; column++){
        value =  values[row][column];
            validate += value;
    }
    if (validate != 45){
        return false;
    }
    else
        return true;
}

bool sub_validator(int column, int row){
    int i, j, value, validate = 0;
    for (i = row; i < row+3; i++){
        for (j = column; j < column+3; j++){
            value =  values[i][j];
            if (0 < value && value <= 9){
                validate += value;
            }else
                return false;
        }
    }
    if (validate == 45){
        return true;
    }
    return false;
}

void *column_validator(void *arg){
    omp_set_num_threads(9);
    omp_set_nested(true);
    printf("El thread que ejecuta el metodo para ejecutar el metodo de revision de columnas es: %ld\n", syscall(SYS_gettid));
    int row, column, validate;
    struct validation *vc;
    vc = (struct validation*)arg;
    #pragma omp parallel for default(none) shared(values, vc) private(row, validate)
        for (column = 0; column < SUDOKU_SIZE; column++){
            validate = 0;
            for (row = 0; row < SUDOKU_SIZE; row++){
                validate += values[row][column];
            }
            if (validate != 45)
                vc->column[column] = false;
            else
                vc->column[column] = true;
            printf("En la revision de columnas el siguiente es un thread en ejecucion %ld\n", syscall(SYS_gettid));
        }
    pthread_exit(0);
}

int main(int argc, char*argv[]) {
//    omp_set_num_threads(1);
    omp_set_nested(true);
    /* Variables */
    int sudoku_fd, status, value, i, row, column, count = 0;
    char ppid[12]; /* No estoy segura de quemar el length */
    char* map_pointer;
    char*  sudoku_file = argv[1];
    struct stat st;
    int file_size;
    pid_t pid, parent_id;
    bool valid_sudoku;
    pthread_t column_thread;
    struct validation validate;

    /* Open the file to read the possible solution*/
    sudoku_fd = open(sudoku_file, O_RDONLY, 0);
    /* Checking if an error occur  */
    if(sudoku_fd == -1){
        perror("open");
        close(sudoku_fd);
        return EXIT_FAILURE;
    }

    /* Obtaining the size of the file */
    status = fstat(sudoku_fd, &st);
    if(status < 0){
        perror("fstat");
        close(sudoku_fd);
        return EXIT_FAILURE;
    }
    file_size = st.st_size;

    /* Mapping the file to a pointer */
    /* To later could read the memory it is necessary to add the MAP_PRIVATE flag */
    map_pointer = mmap(NULL, file_size, PROT_READ, MAP_FILE | MAP_PRIVATE, sudoku_fd, 0);
    if(map_pointer == MAP_FAILED){
        perror("mmap");
        close(sudoku_fd);
        return EXIT_FAILURE;
    }

    /* Reading the file */
    for (i = 0; i < file_size-1; i++) {
        value = (int)(map_pointer[i]) - 48; /* Converting the ASCII to decimal */
        /* Filling the 2D-array */
        values[(int)(i / SUDOKU_SIZE)][(int)(i % SUDOKU_SIZE)] = value;
    }

    /* Validating the sub_arrays */
    #pragma omp parallel for default(none) shared(validate)
    for (i = 0; i < SUDOKU_SIZE; i++) {
       validate.array[i] = sub_validator((i%3)*3, (int)(i/3)*3);
    }


    parent_id = getpid();
    pid = fork();
    switch (pid) {
        case -1:
            perror("fork");
            close(sudoku_fd);
            return EXIT_FAILURE;
        case 0: /*  The child process */
            sprintf(ppid, "%d", parent_id);
            /* Information about the parent process */
            execlp("/bin/ps","ps", "-p", ppid, "-lLf", (char *) NULL); /* No me funciona aún con el numero de proceso */
            break;
        default: /* The parent process */
            /* TODO check that the thread was correctly created */
            pthread_create(&column_thread, NULL, column_validator, (void*)&validate);
            pthread_join(column_thread, NULL);
            /* Main thread */
            printf("El thread en el que se ejecuta main es: %ld\n", syscall(SYS_gettid));
            /* Wait for the child */
            wait(0);

            /* Validation by rows */
            #pragma omp parallel for default(none) shared(validate)
                for (row = 0; row < SUDOKU_SIZE; row++){
                    validate.row[row] = checkRow(row);
                }

            /* Checking all the validation made before */
            for (i = 0; i < SUDOKU_SIZE; i++){
                if (validate.column[i] == false || validate.row[i] == false || validate.array[i] == false){
                    valid_sudoku = false;
                    break;
                }
                valid_sudoku = true;
            }

            /* Printing if the solution is valid */
            if (valid_sudoku)
                printf("%s---- * The solution is VALID * ----%s\n", GRN, NRM);
            else
                printf("%s---- * The solution is INVALID * ----%s\n", RED, NRM);

            pid = fork();
            switch (pid) {
                case -1:
                    perror("fork");
                    close(sudoku_fd);
                    return EXIT_FAILURE;
                case 0: /*  The child process */
                    sprintf(ppid, "%d", parent_id);
                    /* Information about the parent process */
                    execlp("/bin/ps","ps", "-p", ppid, "-lLf", (char *) NULL);
                    break;
                default: /* The parent process */
                    wait(0);
                    break;
            }
            break;
    }

    return 0;
}
