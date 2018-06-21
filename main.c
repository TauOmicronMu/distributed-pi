#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <float.h> 

const long double TOTAL_WORK = 99.0e50;
const long double  NUM_WORKERS = 99.0;
const long double WORK_PER_WORKER = 1.0e50;
long double PI = 0.0;

int client(long start_n);
int server(void);
long double calc(long start_n);

/*
    The server holds an int, and listens for read/write
    requests from clients. 
*/
int server(void) {
     
    long double WORKERS_FINISHED = 0.0;
   
    while(WORKERS_FINISHED < NUM_WORKERS) {
        // Wait for a worker to respond with their workload and add it to PI
        MPI_Status status;
        long double response;
        MPI_Recv(&response, 1, MPI_LONG_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        PI += response;
        WORKERS_FINISHED++;
    }

    printf("Pi is: %.*Lf\n", LDBL_DECIMAL_DIG, (long double)((4.0*PI)/TOTAL_WORK));
    
    return 0;
}

/*
   The client asynchronously reads and writes to the int
   stored in the server. 
*/ 
int client(long start_n) {

    // Calculate pi between n and n + WORK_PER_WORKER
    long double retval = calc(start_n);
     
    // Send retval back to the server
    MPI_Send(&retval, 1, MPI_LONG_DOUBLE, 0, 0, MPI_COMM_WORLD);

    return 0;
}

long double calc(long start_n) {
    long double a = 0.0;
    long double b = 0.0;
    long double c = 0.0;
    long double calc = 0.0;
    for (long i = start_n; i < start_n + (long)WORK_PER_WORKER; i++) { 
        c = ((long double)i - 0.5) / TOTAL_WORK;
        b = pow(c, 2.0);
        a = 1.0 + b;
        calc += 1.0/a;
    }
    return calc;
}

int main(int argc, char ** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0) server();
    else {
        client(WORK_PER_WORKER * (long double)(rank - 1));
    }

    MPI_Finalize();

    return 0;
}

