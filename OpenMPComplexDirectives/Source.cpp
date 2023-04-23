#include <stdio.h>
#include <omp.h>

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
	for (long i = 1; i < N; i++) {
		if (isPrime(i)) {
			counter += 1;
		}
	}

	return counter;
}

//version 1 with OpenMP

//counter is shared netween all threads -> racing conditions
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

int main() {

	const int N = 4e5;
	int counter = 0;

	printf("\n Start counting ...");

	double tStart = omp_get_wtime();

	counter = ompSolutionV2(N);

	double tFinal = omp_get_wtime();

	printf("\n Number of primes = %d in %f seconds", counter, tFinal - tStart);
}