/*
 * algorithm: odd-even transposition sort (alg. ~40 lines long)
 * author: jakub zak
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

void be_passive(int my_values_c, int **mynumbers, int nei_id){
    // Inform how many values I own
    MPI_Send(&my_values_c, 1, MPI_INT, nei_id, TAG, MPI_COMM_WORLD);
    //Send all my values to  neighbour
    for (int j = 0; j < my_values_c; j++)
        MPI_Send(&(*mynumbers)[j], 1, MPI_INT, nei_id, TAG, MPI_COMM_WORLD);
    for (int j = 0; j < my_values_c; j++)
        MPI_Recv(&(*mynumbers)[j], 1, MPI_INT, nei_id, TAG, MPI_COMM_WORLD, NULL);
}

void be_aggressive(int my_values_c, int **mynumbers, int nei_id){
    //Ask how many values my right neighbour holds
    int nei_count;
    MPI_Recv(&nei_count, 1, MPI_INT, nei_id, TAG, MPI_COMM_WORLD, NULL);

    //Read values form right neighbour and merge in one list
    int* merged = new int [my_values_c + nei_count];
    int my_c_id = 0;
    int nei_c_id = 1;
    int cur_id = 0;
    int cur_read;
    MPI_Recv(&cur_read, 1, MPI_INT, nei_id, TAG, MPI_COMM_WORLD, NULL);
    while (cur_id < my_values_c + nei_count){
        if (my_c_id < my_values_c && (*mynumbers)[my_c_id] < cur_read)
            merged[cur_id++] = (*mynumbers)[my_c_id++];
        else {
            merged[cur_id++] = cur_read;
            if (nei_c_id < nei_count){
                MPI_Recv(&cur_read, 1, MPI_INT, nei_id, TAG, MPI_COMM_WORLD, NULL);
                nei_c_id++;
            }
            else
                cur_read = INT_MAX;
        }
    }
    // Keep first half for yourself
    cur_id = 0;
    while (cur_id < my_values_c){
        (*mynumbers)[cur_id] = merged[cur_id];
        cur_id++;
    }
    // Send second half to the right neighbour
    while (cur_id < my_values_c + nei_count)
        MPI_Send(&merged[cur_id++], 1, MPI_INT, nei_id, TAG, MPI_COMM_WORLD);
}


int main(int argc, char *argv[])
{
    int* mynumbers;             // My values
    int numprocs;               // Number of processors
    int myid;                   // My rank

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    int values_n = stoi(argv[1]);     // How many numbers are there
    // If there is less values than processors, do not care about last n at all
    if (values_n < numprocs){
        numprocs = values_n;
        if (myid >= numprocs){
            MPI_Finalize();
            return 0;
        }
    }
    int max_np = values_n/numprocs;   // Max in one processor
    int c_max;

    /* File reading
     * proc 0 reads the file and sends appropriate amount of number to all processors
     */
    if(myid == 0){
        fstream fin;
        fin.open("numbers", ios::in);

        for (int invar = 0; invar < numprocs; invar++){
            if (invar < values_n%numprocs)
                c_max = max_np + 1;
            else
                c_max = max_np;

            for (int i = 0; i < c_max; i++){
                int c = fin.get();
                cout<<c<<" ";
                MPI_Send(&c, 1, MPI_INT, invar, TAG, MPI_COMM_WORLD);
            }
        }
        fin.close();
        cout<<endl;
    }

    // Count how many values I own
    if (myid < values_n%numprocs)
        c_max = max_np + 1;
    else
        c_max = max_np;

    // Receive my init values
    mynumbers = new int [c_max];
    for (int i = 0; i < c_max; i++)
        MPI_Recv(&mynumbers[i], c_max, MPI_INT, 0, TAG, MPI_COMM_WORLD, NULL);

    // Sort my initial values
    sort(mynumbers, mynumbers+c_max);

    for (int i = 0; i < ceil(numprocs/2.0) + 1; i++){
        // even
        if (myid % 2 == 0){
            if (myid < numprocs - 1)
                be_aggressive(c_max, &mynumbers, myid+1);
            if (myid != 0)
                be_passive(c_max, &mynumbers, myid-1);
        }
        // odd
        else {
            // Step 1
            be_passive(c_max, &mynumbers, myid-1);
            if (myid < numprocs - 1)
                be_aggressive(c_max, &mynumbers, myid+1);
        }
    }

    // Inform the 0 about my result
    MPI_Send(&c_max, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
    for (int i = 0; i < c_max; i++)
        MPI_Send(&mynumbers[i], 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);

    // If I am the 0, then print result
    if (myid == 0){
        for (int i = 0; i < numprocs; i++){
            int count;
            int val;
            MPI_Recv(&count, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, NULL);
            for (int j = 0; j < count; j++){
                MPI_Recv(&val, 1, MPI_INT, i, TAG, MPI_COMM_WORLD, NULL);
                cout<<val<<endl;
            }
        }
    }

    MPI_Finalize();
    return 0;
}
