#include "KernelSemaphore.h"

using namespace std; 

int main(){
	cout<<"This program will create the semaphore, initialize it to 0, ";
	cout<<"then produce some data and finally V() the semaphore"<<endl;
	KernelSemaphore ks ("/my_kernel_sema", 0);
	sleep (rand() % 10);// sleep a random amount of seconds
	ks.V();
}