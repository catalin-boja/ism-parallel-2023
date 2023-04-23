#include <stdio.h>
#include <omp.h>
#include <vector>

bool isPrime(long value) {
	for (long i = 2; i < value/2; i++) {
		if (value % i == 0)
			return false;
	}
	return true;
}

int standardSolution(const int N) {
	int counter = 0;

	//standard solution  to count number of primes from 1 to N
	for (long i = 0; i < N; i++) {
		if (isPrime(i)) {
			counter += 1;
		}
	}

	return counter;
}

//version 1 with OpenMP

//counter is shared between all threads -> racing conditions
//thre for loop is copy-pasted in all threads -> all threads process the same interval
int ompSolutionV1(const int N) {
	int counter = 0;

#pragma omp parallel 
	{
		for (long i = 1; i < N; i++) {
			if (isPrime(i)) {
				counter += 1;
			}
		}
	}

	return counter;
}

//version 2 with OpenMP

//counter is shared between all threads -> racing conditions

int ompSolutionV2(const int N) {
	int counter = 0;

#pragma omp parallel 
	{
		int size = N / omp_get_num_threads();
		long lowerLimit = omp_get_thread_num() * size;
		long upperLimit = 
			(omp_get_thread_num() == omp_get_num_threads() - 1) ? 
				N : (omp_get_thread_num() + 1) * size;

		for (long i = lowerLimit; i < upperLimit; i++) {
			if (isPrime(i)) {
				counter += 1;
			}
		}

	}

	return counter;
}

//version 3 with OpenMP

//solving racing condition with a mutex

int ompSolutionV3(const int N) {
	int counter = 0;

	omp_lock_t mutex;
	omp_init_lock(&mutex);

#pragma omp parallel 
	{
		int size = N / omp_get_num_threads();
		long lowerLimit = omp_get_thread_num() * size;
		long upperLimit =
			(omp_get_thread_num() == omp_get_num_threads() - 1) ?
			N : (omp_get_thread_num() + 1) * size;

		for (long i = lowerLimit; i < upperLimit; i++) {
			if (isPrime(i)) {

				//making this op an atomic operation
				omp_set_lock(&mutex);
				counter += 1;
				omp_unset_lock(&mutex);
			}
		}
	}

	return counter;
}

//version 4 with OpenMP
//avoiding shared variables

int ompSolutionV4(const int N) {
	int counter = 0;

	std::vector<int> results(omp_get_num_procs());

#pragma omp parallel num_threads(omp_get_num_procs()) private(counter)
	{
		counter = 0;
		int size = N / omp_get_num_threads();
		long lowerLimit = omp_get_thread_num() * size;
		long upperLimit =
			(omp_get_thread_num() == omp_get_num_threads() - 1) ?
			N : (omp_get_thread_num() + 1) * size;

		for (long i = lowerLimit; i < upperLimit; i++) {
			if (isPrime(i)) {
				counter += 1;
			}
		}

		results[omp_get_thread_num()] = counter;
	}

	for (int i = 0; i < omp_get_num_procs(); i++) {
		counter += results[i];
	}

	return counter;
}

//version 5 - using parallel for with local results

int ompSolutionV5(const int N) {
	int counter = 0;
	std::vector<int> results(omp_get_num_procs());

#pragma omp parallel private(counter)
	{
#pragma omp for
		for (long i = 0; i < N; i++) {
			if (isPrime(i)) {
				counter += 1;
			}
		}
		results[omp_get_thread_num()] = counter;
	}

	for (int i = 0; i < omp_get_num_procs(); i++) {
		counter += results[i];
	}

	return counter;
}

//version 6 - using parallel for with reduction

int ompSolutionV6(const int N) {
	int counter = 0;

#pragma omp parallel shared(counter)
	{
#pragma omp for reduction(+: counter)
		for (long i = 0; i < N; i++) {
			if (isPrime(i)) {
				counter += 1;
			}
		}
	}

	return counter;
}

//version 7 - using parallel for with reduction

int ompSolutionV7(const int N) {
	int counter = 0;

#pragma omp parallel for reduction(+: counter)
		for (long i = 0; i < N; i++) {
			if (isPrime(i)) {
				counter += 1;
			}
		}
	return counter;
}

//version 8 - using parallel for with reduction and different scheduler

int ompSolutionV8(const int N) {
	int counter = 0;

#pragma omp parallel for schedule(dynamic, 10) reduction(+: counter)
	for (long i = 0; i < N; i++) {
		if (isPrime(i)) {
			counter += 1;
		}
	}
	return counter;
}

int main() {

	const int N = 4e5;
	int counter = 0;

	printf("\n Start counting ...");

	double tStart = omp_get_wtime();

	counter = ompSolutionV8(N);

	double tFinal = omp_get_wtime();

	printf("\n Number of primes = %d in %f seconds", counter, tFinal - tStart);
}