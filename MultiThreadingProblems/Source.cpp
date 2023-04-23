#include <thread>
#include <string>
#include <vector>
#include <mutex>
#include <omp.h>

using namespace std;

//define a global mutex
mutex counterMutex;

void counter(int& counter, int limit) {
	for (int i = 0; i < limit; i++)
		counter += 1;
}

void counterWithMutex(int& counter, int limit) {
	for (int i = 0; i < limit; i++)
	{
		counterMutex.lock();
		counter += 1;
		counterMutex.unlock();
	}
}

int main() {

	const int noCores = omp_get_num_procs();
	printf("\n Available cores: %d", noCores);

	double currentTime = omp_get_wtime();

	printf("\n Current time %f", currentTime);


	int finalCounter = 0;
	vector<thread> threads;

	const int limit = 5e8;

	//double tStart = omp_get_wtime();

	//for (int i = 0; i < noCores; i++) {
	//	threads.push_back(thread(counter, ref(finalCounter), limit));
	//}
	//for (int i = 0; i < noCores; i++) {
	//	threads[i].join();
	//}

	//double tFinal = omp_get_wtime();

	//printf("\n The counter result with race condition and multiple threads is %d and it required %f s",
	//	finalCounter, (tFinal - tStart));

	//
	//testing the solution with a mutex and without race conditions
	//

	//threads.clear();

	//tStart = omp_get_wtime();
	//finalCounter = 0;

	//for (int i = 0; i < noCores; i++) {
	//	threads.push_back(thread(counterWithMutex, ref(finalCounter), limit));
	//}
	//for (int i = 0; i < noCores; i++) {
	//	threads[i].join();
	//}

	//tFinal = omp_get_wtime();

	//printf("\n The counter result with mutex and multiple threads is %d and it required %f s",
	//	finalCounter, (tFinal - tStart));



	//testing without mutex and race conditions with multiple variables
	threads.clear();
	finalCounter = 0;
	int* threadResults = new int[noCores];
	for (int i = 0; i < noCores; i++) {
		threadResults[i] = 0;
	}

	double tStart = omp_get_wtime();

	for (int i = 0; i < noCores; i++) {
		threads.push_back(thread(counter, ref(threadResults[i]), limit));
	}
	for (int i = 0; i < noCores; i++) {
		threads[i].join();
	}
	double tFinal = omp_get_wtime();

	for (int i = 0; i < noCores; i++) {
		finalCounter += threadResults[i];
	}

	printf("\n The counter result with mutex and multiple threads is %d and it required %f s",
		finalCounter, (tFinal - tStart));



	//testing without mutex and race conditions with multiple variables on different cache lines
	threads.clear();
	finalCounter = 0;

	//because I have 8 cores
	int threadResultsMatrix[8][1000];

	for (int i = 0; i < noCores; i++) {
		threadResultsMatrix[i][0] = 0;
	}

	tStart = omp_get_wtime();

	for (int i = 0; i < noCores; i++) {
		threads.push_back(thread(counter, ref(threadResultsMatrix[i][0]), limit));
	}
	for (int i = 0; i < noCores; i++) {
		threads[i].join();
	}
	tFinal = omp_get_wtime();

	for (int i = 0; i < noCores; i++) {
		finalCounter += threadResultsMatrix[i][0];
	}

	printf("\n The counter result without cache coherence is %d and it required %f s",
		finalCounter, (tFinal - tStart));


}