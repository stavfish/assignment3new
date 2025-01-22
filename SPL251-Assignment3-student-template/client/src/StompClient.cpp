#include <mutex>
#include <iostream>
#include <thread>

std::mutex locker;
int global;
void inc(){
	locked.lock();
	global = global +1;
	locker.unlock();
}

int main(int argc, char *argv[]) {
	// TODO: implement the STOMP client
	std:: thread t1 {inc};
	std:: thread t2 {inc};
	t1.join();
	t2.join();

	return 0;
}