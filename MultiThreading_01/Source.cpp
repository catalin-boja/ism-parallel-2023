#include <stdio.h>
#include <thread>
using namespace std;

void hello(int timeout, int id) {
	this_thread::sleep_for(
		std::chrono::seconds(timeout));
	printf("\n Hello from thread %d!", id);
}

void arraySum(int* array, int n, int& result) {
	int sum = 0;
	for (int i = 0; i < n; i++) {
		sum += array[i];
	}
	result = sum;
}

int main() {

	printf("\n Hello ! Starting main thread");

	printf("\n Hello from 1");
	printf("\n Hello from 2");

	std::thread t1(hello, 2, 1);
	std::thread t2(hello, 4, 2);

	//getting results from threads using references
	int values[] = { 1,2,3,4,5,6 };
	int n = 6;

	int result = 0;

	std::thread t3(arraySum, values, n, std::ref(result));

	printf("\n Bye ! Ending main thread");

	//sync secondary threads 
	t1.join();
	t2.join();
	t3.join();

	printf("\n Array sum is %d", result);
}