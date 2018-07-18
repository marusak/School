/*
 * algorithm: Preorder tree tour
 * author: Matej Marusak, xmarus06
 *
 */

#include <mpi.h>
#include <math.h>
#include <limits.h>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

#define TAG 0

bool is_leaf(int index, int n){
    return index >= n/2 && index < n;
}

bool is_forward(int index, int n){
    return index < n;
}

bool has_right(int index, int n){
    return !is_forward(index,n) && ((index-n) % 2 == 0) && (index < 2*n - 2);
}

int main(int argc, char *argv[])
{
    int* mynumbers;             // My values
    int numprocs;               // Number of processors
    int myid;                   // My rank

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (numprocs < 4){
        if (myid == 0)
            cout<<argv[1];
        MPI_Finalize();
        return 0;
    }


    // Lets count from 1
    myid++;

    // Count how many vertices are there
    int n = (numprocs + 2) / 2;

    int* arr = new int[numprocs];

    // Find Euler tour
    // Find what follows after me and inform all about that
    int new_value;
    if (is_forward(myid,n)){
        if (is_leaf(myid,n))
            new_value = myid + n - 1;
        else
            new_value = 2 * myid + 1;
    } else {
        if (has_right(myid,n))
            new_value = myid - n + 2;
        else
            new_value = (n - 2) / 2 + (myid / 2);
    }
    //root
    if (myid == n+1)
        new_value = myid;
    MPI_Allgather(&new_value, 1, MPI_INT, arr, 1, MPI_INT, MPI_COMM_WORLD);

    // Find weights
    int* arr_weights = new int[numprocs];
    int weight = 0;
    if (is_forward(myid, n))
        weight = 1;
    MPI_Allgather(&weight, 1, MPI_INT, arr_weights, 1, MPI_INT, MPI_COMM_WORLD);

    // Count suffix
    int* arr_final = new int[numprocs];
    int forward_edges = 0;
    int i = myid - 1;
    while (arr[i] != i + 1){
        if (arr_weights[i])
            forward_edges++;
        i = arr[i] - 1;
    }

    MPI_Allgather(&forward_edges, 1, MPI_INT, arr_final, 1, MPI_INT, MPI_COMM_WORLD);

    if(myid == 1){
        char* letters = new char[n+1];
        letters[0] = argv[1][0];
        for(int i=0; i < n - 1; i++){
            letters[n-arr_final[i]] = argv[1][i+1];
        }
        letters[n] = '\0';
        cout<<letters;
        delete letters;
    }
    delete arr;
    delete arr_weights;
    delete arr_final;

    MPI_Finalize();
    return 0;
}
