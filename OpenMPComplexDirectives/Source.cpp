#include <stdio.h>
#include <omp.h>

bool isPrime(long value) {
	for (long i = 2; i < value/2; i++) {
		if (value % i == 0)
			return false;
	}
	return true;
}

int main() {

	const int N = 4e5;
	int counter = 0;

	printf("\n Start counting ...");

	double tStart = omp_get_wtime();

	//standard solution  to count number of primes from 1 to N
	for (long i = 1; i <= N; i++) {
		if (isPrime(i)) {
			counter += 1;
		}
	}

	double tFinal = omp_get_wtime();

	printf("\n Number of primes = %d in %f seconds", counter, tFinal - tStart);
}