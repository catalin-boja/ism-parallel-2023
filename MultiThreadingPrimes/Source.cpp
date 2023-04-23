#include <stdio.h>
#include <thread>
#include <omp.h>
#include <string>
#include <vector>
#include <mutex>
using namespace std;

bool isPrime(long value) {
	bool isOk = true;
	for (int i = 2; i <= value / 2; i++) {
		if (value % i == 0) {
			isOk = false;
			break;
		}
	}
	return isOk;
}

void benchmark(
	string msg,
	long lowerLimit,
	long upperLimit, int (*pf)(long, long))
{
	printf("\n %s", msg.c_str());
	double tStart = omp_get_wtime();
	int result = pf(lowerLimit, upperLimit);
	double tFinal = omp_get_wtime();
	printf("\n Result = %d in %f seconds",
		result, tFinal - tStart);
}

//sequential version - counting primes
int seqPrimes(long lower, long upper) {
	int result = 0;
	for (long value = lower; value <= upper; value += 1) {
		if (isPrime(value)) {
			result += 1;
		}
	}
	return result;
}



void seqPrimesRef(long lower, long upper, int& result) {
	double tStart = omp_get_wtime();
	
	for (long value = lower; value <= upper; value += 1) {
		if (isPrime(value)) {
			result += 1;
		}
	}
	double tFinal = omp_get_wtime();

	printf("\n Routine duration = %f seconds", 
		(tFinal - tStart));
}

void seqPrimesRefWithStep(long lower, long upper, int& result, int step) {
	double tStart = omp_get_wtime();

	printf("\n Lower: %d, step = %d", lower, step);
	for (long value = lower; value <= upper; value += step) {
		if (isPrime(value)) {
			result += 1;
		}
	}
	double tFinal = omp_get_wtime();

	printf("\n Routine duration = %f seconds",
		(tFinal - tStart));
}

void seqPrimesRefSync(long lower, long upper, mutex& accessToken, int& result) {
	for (long value = lower; value <= upper; value += 1) {
		if (isPrime(value)) {
			accessToken.lock();
			result += 1;
			accessToken.unlock();
		}
	}
}

int parallelPrimes(long lower, long upper) {
	int noCores = omp_get_num_procs();
	printf("\n Available cores = %d", noCores);
	vector<thread> threads;
	int intervalSize = (upper - lower) / noCores;


	int result = 0;

	for (int i = 0; i < noCores; i++) {
		long intervalLower = lower + i * intervalSize;
		long intervalUpper = (i + 1) * intervalSize;
		if (i == noCores - 1)
			intervalUpper = upper;

		threads.push_back(thread (seqPrimesRef,
			intervalLower,
			intervalUpper,
			ref(result)));
	}

	for (int i = 0; i < noCores; i++) {
		threads[i].join();
	}

	return result;
}

int parallelPrimesSync(long lower, long upper) {
	int noCores = omp_get_num_procs();
	printf("\n Available cores = %d", noCores);
	vector<thread> threads;
	int intervalSize = (upper - lower) / noCores;


	int result = 0;
	mutex token;

	for (int i = 0; i < noCores; i++) {
		long intervalLower = lower + i * intervalSize;
		long intervalUpper = (i + 1) * intervalSize;
		if (i == noCores - 1)
			intervalUpper = upper;

		threads.push_back(thread(
			seqPrimesRefSync,
			intervalLower,
			intervalUpper,
			ref(token),
			ref(result)));
	}

	for (int i = 0; i < noCores; i++) {
		threads[i].join();
	}

	return result;
}

int parallelPrimesWithoutRaceCond(long lower, long upper) {
	int noCores = omp_get_num_procs();
	printf("\n Available cores = %d", noCores);

	vector<thread> threads;
	vector<int> localResults(noCores);

	int intervalSize = (upper - lower) / noCores;

	int result = 0;

	for (int i = 0; i < noCores; i++) {
		long intervalLower = lower + i * intervalSize;
		long intervalUpper = (i + 1) * intervalSize;
		if (i == noCores - 1)
			intervalUpper = upper;

		localResults[i] = 0;

		threads.push_back(thread(seqPrimesRef,
			intervalLower,
			intervalUpper,
			ref(localResults[i])));
	}

	for (int i = 0; i < noCores; i++) {
		threads[i].join();
		result += localResults[i];
	}

	return result;
}

int parallelPrimesWithLoadBalancing(long lower, long upper) {
	int noCores = omp_get_num_procs();
	printf("\n Available cores = %d", noCores);

	vector<thread> threads;
	vector<int> localResults(noCores);

	int result = 0;

	for (int i = 0; i < noCores; i++) {
		localResults[i] = 0;

		threads.push_back(thread(seqPrimesRefWithStep,
			lower % 2 == 0 ? lower + 2*i + 1 : lower + 2*i,
			upper,
			ref(localResults[i]),
			2*noCores));
	}

	for (int i = 0; i < noCores; i++) {
		threads[i].join();
		result += localResults[i];
	}

	return result;
}

int main() {
	const long upper = 4e5;
	benchmark("Classic approach", 1, upper, seqPrimes);
	benchmark("Parallel approach ver. 1", 1, upper, parallelPrimes);
	benchmark("Parallel approach ver. 2 with sync", 1, upper, parallelPrimesSync);
	benchmark("Parallel approach ver. 3 with local variables", 1, upper, parallelPrimesWithoutRaceCond);
	benchmark("Parallel approach ver. 4 with load balancing", 1, upper, parallelPrimesWithLoadBalancing);
}