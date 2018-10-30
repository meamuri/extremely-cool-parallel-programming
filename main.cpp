#include <iostream>  
#include "mpi.h"
#include "stdio.h"
#include "ctime"
#include "stdlib.h"

/*
Лаб 1, Задача 6. 
Найти max X(i, j), X(i + 1, j), j = 0, ... m (где m - размер локального вектора одного процесса).
Два соседних процесса обмениваются сообщениями, один из них вычисляет локальный max и выводит результат.
*/

const int N = 10; //Размер вектора
int main(int arg0, char * arg1[]) {
	int rank,          //Ранг процесса
		numprocs;      //Количество процессов

	MPI_Init(&arg0,&arg1);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	//Инициализация генератора случайных чисел
	srand(time(NULL) - rank * 2);

	//Формируем случайный вектор длины N и выводим его
	int Vector[N];
	printf("\nProcess %d : \t\t",rank);
	for (int i=0; i<N; i++)
		printf("%d ",Vector[i] = rand() % N);
	printf("\n");

	///Процесс с rank = 0 передает процессу с rank=1 сообщение о значениях вектора, с rank=2 => rank=4;

	if (rank % 2 != 0) { //Нечетные процессы получают
		int value; //Полученное значение
		MPI_Status status;
		for (int i = 0 ; i < N ; i++) {
			int TAG = i;
			MPI_Recv(&value, 1, MPI_INT, rank-1, TAG, MPI_COMM_WORLD, &status);
			if (Vector[i] < value)
				Vector[i] = value;
		}

		//Выводим итоговый вектор
		printf("\n For processes %d ; %d :\t", rank-1 ,rank );
		for (int i = 0 ; i < N ; i++) 
			printf("%d ", Vector[i]);
		printf("\n");
	}
	else //Четные процессы отправляют
		if (rank != numprocs-1) {  //Если последний процесс четный, он ничего не должен отправлять
			for (int i = 0;i<N;i++) {
				int TAG=i;
				MPI_Send( &Vector[i] , 1, MPI_INT, rank+1, TAG, MPI_COMM_WORLD);
			}
		}
	MPI_Finalize();
	return 0;
}