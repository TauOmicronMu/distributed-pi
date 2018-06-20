#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <pthread.h>

const size_t NUM_WORKERS = 99;
size_t SERVER_FINISHED = 0;

int client(void);
int server(void);
int handleRequest(MPIStatus);

int main(int argc, char ** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0) server();
    else client();

    MPI_Finalize();

    return 0;
}

/*
    The server has some central data structure, and the clients can make requests that alter or return that
    data structure.
*/
int server(void) {
    size_t WORKERS_ONGOING = NUM_WORKERS;
 
    // TODO: work out how to do ongoing workers?
    while(!SERVER_FINISHED) { 
        // TODO: add a check to see if the work is done!
        // NB: this should alter SERVER_FINISHED

        // Wait for a client to send a request and handle it in a new thread
        size_t result;
        MPI_Status status;
        MPI_Recv(&result, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
   
        // Spin up a new pthread to handle the client request 
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handleRequest, &status);
        pthread_join(client_thread, NULL);         
 
    }     

    // Once the server has finished, wait for all 99 clients to poll the server for status
    while(WORKERS_ONGOING > 0) {
        // Wait for a client to send a request and handle it in a new thread
        size_t result;
        MPI_Status status;
        MPI_Recv(&result, 1, MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // We don't care about any requests that aren't polling for status
        if(status.MPI_TAG != 0) continue;

        // Spin up a new pthread to handle the client request 
        pthread_t client_thread;
        pthread_create(&client_thread, NULL, handleRequest, &status);
        pthread_join(client_thread, NULL);
        
        WORKERS_ONGOING--;
    }
  
    // TODO: provide any required output before the program exits

    return 0;
}

/* 
    Handles a client request 
*/
int handleRequest(MPI_Status status) {
    switch(status.MPI_TAG) {
        // TODO: Add cases here for each requests
        case 0:
            // In this case, the client is polling to see if the work is done.
            // Hence, return the SERVER_FINISHED int (0 if not done, 1 if done)    
            MPI_Send(&SERVER_FINISHED, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);
            return 0;
        default:
            return 0;
    }    
}


/*
   The client sends requests to the server
*/ 
int client(void) {
    size_t WORK_FINISHED = 0; 
 
    while(!WORK_FINISHED) {
        // First, poll the server to check we still have work!
        MPI_Send(&WORK_FINISHED, 1, MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);
        
        // Then get the result from the server, and update WORK_TO_BE_DONE
        MPI_Status status;
        MPI_Recv(&WORK_FINISHED, 1, MPI_UNSIGNED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // TODO: Do any required work
    }

    return 0;
}
