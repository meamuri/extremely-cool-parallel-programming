/**
 *
 * Каждый процесс заполняет свой массив размера n случайными числами.
 * Для решения задачи использовать операции приведения с собственной функцией для решения задачи.
 * Результат - вектор размера n, каждый элемент которого получен по следующему правилу:
 *
 * Найти количество нулей
 */

#include <iostream>
#include "mpi.h"

using namespace std;

const int N = 4;

const int MASTER_ID = 0;

void findZeroCount(int *in, int *zeroCount, int *len, MPI_Datatype *datatype);

int main(int argc, char *argv[]) {

    int rank, processCount;
    int *arr = new int[N];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Генерация и печать массива для текущего процесса
    srand(time(0) + rank * 10);
    for (int i = 0; i < N; i++) {
        arr[i] = -N / 2 + rand() % (2 * N);
    }

    int *recvbuf = new int[N * processCount];
    MPI_Gather(arr, N, MPI_INT, recvbuf, N, MPI_INT, MASTER_ID, MPI_COMM_WORLD);
    if (rank == MASTER_ID) {
        for (int i = 0; i < processCount; i++) {
            cout << "Process " << i << ":\t";
            for (int j = 0; j < N; j++) {
                cout << recvbuf[N * i + j] << "\t";
            }
            cout << endl;
        }
    }

 	int *result = new int[N];
    MPI_Op operation;
    MPI_Op_create((MPI_User_function *) findZeroCount, 0, &operation);
    MPI_Reduce(arr, result, N, MPI_INT, operation, 0, MPI_COMM_WORLD);

    if (rank == MASTER_ID) {
        cout << "Result:\t\t";
        for (int i = 0; i < N; i++)
            cout << result[i] << "\t";
        cout << endl;
    }

    MPI_Op_free(&operation);
    MPI_Finalize();
    return 0;
}

void findZeroCount(int *in, int *zeroCount, int *len, MPI_Datatype *datatype) {
    for (int i=0; i<*len; i++) {
 		if (in[i] == 0) {
			zeroCount[i]++;
		}
	}
}
