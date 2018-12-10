#include <iostream>
#include <mpi.h>

using namespace std;

#define N 10 // Размерность матрицы

/**
 * УСЛОВИЕ ЗАДАЧИ:
 *
 * Создать описатель типа и использовать его при передаче данных
 * в качестве шаблона для следующего преобразования:
 *
 * первая строка, первый столбец, элементы над побочной диагональю.
 *
 * + + + + +
 * + + + + -
 * + + + - -
 * + + - - -
 * + - - - -
 *
 */

const int MASTER_ID = 0; // Нулевой процесс
const int WORKER_ID = 1;

const string BEFORE_MATRIX_MESSAGE = "Matrix before:";
const string AFTER_MATRIX_MESSAGE = "Matrix after:";

void printMatrix(int matrix[N][N], string message);

int main(int argc, char *argv[]) {

    int rank, tag = MASTER_ID;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int blockLengthArray[N];  // Массив, содержащий число элементов в каждом блоке
    int displacementArray[N]; // Массив смещений каждого блока от начала размещения элемента нового типа

    // Инициализация размеров блоков
    for (int i = 0; i < N; i++) {
        blockLengthArray[i] = N - i;
        displacementArray[i] = i * N;
    }

    MPI_Datatype customMatrixType;
    MPI_Type_indexed(N, blockLengthArray, displacementArray, MPI_INT, &customMatrixType);
    MPI_Type_commit(&customMatrixType);

    // Все элементы матрицы равны рангу текущего процесса
    int matrix[N][N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i][j] = rank;
        }
    }

    if (rank == MASTER_ID) {
        MPI_Send(&matrix[0][0], 1, customMatrixType, WORKER_ID, tag, MPI_COMM_WORLD);
    } else if (rank == WORKER_ID) {
        printMatrix(matrix, BEFORE_MATRIX_MESSAGE);
        MPI_Status status;
        MPI_Recv(&matrix[0][0], 1, customMatrixType, MASTER_ID, tag, MPI_COMM_WORLD, &status);
        printMatrix(matrix, AFTER_MATRIX_MESSAGE);
    }

    MPI_Type_free(&customMatrixType);
    MPI_Finalize();
    return 0;
}

void printMatrix(int matrix[N][N], string description) {
    cout << description << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << to_string(matrix[i][j]) << " ";
        }
        cout << endl;
    }
    cout << endl;
}
