#include <stdio.h>
#include <omp.h>

int main() {

	printf("\n Hello. Starting main thread !");
	printf("\n Hello from main thread !");

	int noCores = omp_get_num_procs();

	printf("\n Number of available cores: %d", noCores);



#pragma omp parallel
	{
		//check number of created threads on thread with id 0
		if(omp_get_thread_num() == 0)
		{
			printf("\n Total number of threads = %d", omp_get_num_threads());
			printf("\n Max number of threads = %d", omp_get_max_threads());
		}
		printf("\n Hello  from thread with id %d", omp_get_thread_num());
	}

	int noRequestedThreads = 4;

	//set number of default threads to 4
	omp_set_num_threads(noRequestedThreads);

#pragma omp parallel
	{
		printf("\n How are you thread %d ?", omp_get_thread_num());
	}


#pragma omp parallel num_threads(2)
	{
		printf("\n Executed only by 2 threads with id %d", omp_get_thread_num());
	}


#pragma omp parallel 
	{
		//checking if we have parallelism
		if (omp_in_parallel())
		{
			printf("\n Executed in parallel");
		}
		else
		{
			printf("\n Executed on one thread");
		}
	}

//controlling the parallel section by a variable
	bool weNeedParallelSolution = false;

#pragma omp parallel if(noCores > 4) num_threads(16)
	{
		printf("\nHello from multiple threads");

		//sequence executed once by thread with id = 0
#pragma omp master
		{
			printf("\nI am the master thread");
		}
	}

	printf("\n Bye. Ending main thread !");



}
