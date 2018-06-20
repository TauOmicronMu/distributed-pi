#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

const size_t TOTAL_WORK = 990000;
const size_t NUM_WORKERS = 99;
long long double PI = 0;

long long WORK_PER_WORKER;

int client(long long start_n);
int server(void);
long double calc(long long start_n);

int main(int argc, char ** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    long long N = 0;
    WORK_PER_WORKER = TOTAL_WORK/NUM_WORKERS;

    if(rank == 0) server();
    else {
        client(N);
        N += WORK_PER_WORKER;

    MPI_Finalize();

    return 0;
}

/*
    The server holds an int, and listens for read/write
    requests from clients. 
*/
int server(void) {
     
    long long WORKERS_FINISHED = 0;
   
    while(WORKERS_FINISHED < NUM_WORKERS) {
        // Wait for a worker to respond with their workload and add it to PI
        MPI_Status status;
        long double response;
        MPI_Recv(&response, 1, MPI_LONG_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("Received response from client\n");
        PI += response;
        WORKERS_FINISHED++;
    }

    printf("Pi is: %lld\n", (long long double)((4.0*PI)/N));
    
    return 0;
}

/*
   The client asynchronously reads and writes to the int
   stored in the server. 
*/ 
int client(long long start_n) {
    // Calculate pi between n and n + WORK_PER_WORKER
    long double retval = calc(start_n);
   
    // Send retval back to the server
    MPI_Send(&retval, 1, MPI_LONG_DOUBLE, 0, 0, MPI_COMM_WORLD);

    printf("Client exiting\n");
    return 0;
}

long double calc(long long start_n) {
    long double a,b,c,calc = 0.0;
    for (long long i = start_n; i < start_n + WORK_PER_WORKER; i++) {
        c = ((long double) i - 0.5) / ((long double) TOTAL_WORK);
        b = pow(c, 2.0);
        a = 1.0 + b;
        calc += 1.0/a;
    }
    return calc;
}


