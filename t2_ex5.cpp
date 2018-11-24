// 5.Требуется вычислить значения суммы ряда в n точках хаданного интервала [A,B] с точностью e. 
// Функция: 
// arcsin(x) = x + 
// 				 + (1/2 * x^3/3) +
//				 + (1/2 * 3/4 * x^5/5) +
//				 + (1/2 * 3/4 * 5/6 * x^7/7) +
//				 + ...
// arcsin(x) = x + 
// 				 + (1 * x^3) / (2 * 3) +
//				 + (1 * 3 * x^5) / (2 * 4 * 5) +
//				 + (1 * 3 * 5 * x^7) / (2 * 4 * 6 * 7) +
//				 + ...
#include "mpi.h"
#include <cmath>

const float EPS = 0.0000001;

//функция приближённого вычисления arcsin(x)
float sum_arcsin(float x, float eps)
{
	float xi = x;
	float sum = 0;
	int i=0;
	while (abs(xi) > eps) {
		sum += xi;
		i++;
		xi *= (2*i - 1) * (2*i - 1) * x*x / (2*i+1) / (2 * i);
	}
	return sum;
}

int main(int argc, char* argv[])
{
	int myrank,numprocs,n;
	float e;
	float *sendbuf_x;  //массив,в который мастер запишет иксы(буфер отправки)
	float *readbuf_x;  //буфер приёма иксов
	float *sendbuf_res;//буфер,в который рабочие процессы помещают результат 
	float *readbuf_res;//массив,в который мастер будет собирать результаты

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	if (numprocs<2) MPI_Abort(MPI_COMM_WORLD,0); // Нужно мининмум 2 процесса
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

	if(myrank==0)
	{
		float A=0,B=0;
		printf("Enter A: ");
		scanf("%f",&A);

		printf("Enter B: ");
		scanf("%f",&B);

		printf("Enter epsilon: ");
		do
		scanf("%f",&e);
		while (e<=0);

		printf("Enter N: ");
		do
		scanf("%d",&n);
		while (n<2); 

		sendbuf_x=new float[n];
		float ABN = (float)(B-A)/(n-1); //Расстояние между точками
		for (int i=0; i<n; i++)
			sendbuf_x[i]=A+i*ABN;        // Вычисление всех точек
	}
	else sendbuf_x=NULL;

	MPI_Bcast(&e,1,MPI_FLOAT,0,MPI_COMM_WORLD);//отправить всем эпсилон
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);//отправить всем n
	int add = (int)(n % numprocs != 0); // 1 если есть остаток, 0 если нет
	readbuf_x=new float[n/numprocs+add];
	sendbuf_res=new float[2*(n/numprocs+add)];
	readbuf_res=new float[2*n];
	MPI_Scatter(sendbuf_x,n/numprocs+add,MPI_FLOAT,readbuf_x,n/numprocs+add,MPI_FLOAT,0,MPI_COMM_WORLD);
	
	float x;
	for (int i=0;i<n/numprocs+add;i++) //для каждого икс вычисляем приближённое и точное значение 
	{
		//взять очередной х из буфера 
		x=readbuf_x[i];
		if (abs(x) <=1)
		{
			sendbuf_res[2*i]=sum_arcsin(x, EPS); // в четных ячейках - приближенное значение
			sendbuf_res[2*i+1]=asin(x); // в нечетных ячейках - точное значение
		}
	}
	//собрать результаты вычислений	
	MPI_Gather(sendbuf_res,2*(n/numprocs+add),MPI_FLOAT,readbuf_res,2*(n/numprocs+add),MPI_FLOAT,0,MPI_COMM_WORLD);
	//вывод результата
	if (myrank==0)
	{
		printf("x\t\tSum\t\tExact value\n");
		for (int i=0; i<n;i++)
			printf("%f\t%f\t%f\n",sendbuf_x[i],readbuf_res[2*i],readbuf_res[2*i+1]);
	}
	MPI_Finalize();
	return 0;
}